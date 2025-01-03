import pickle
import socket
import struct
import sys
import time

import cv2
import torch
from ultralytics import YOLO

host = '192.168.1.11'
port = 8188
qtPort = 8187


def start_client(host='127.0.0.1', port=12345):
    print(f"Using torch version: {torch.__version__}")
    adr = '/home/yuchen/Fdisk/Projects/YahBoom/YahBoom/python/best.pt'
    print(f"Load model from: {adr}")
    model = YOLO(adr)
    last_time = time.time()
    sys.stdout.flush()
    while True:
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((host, port))
            print("Connected to the server.")
            sys.stdout.flush()

            # 创建 TCP 客户端套接字（连接到 Qt 程序）
            client_socket_qt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket_qt.connect(("localhost", qtPort))  # 本地端口连接 Qt 程序
            print("Connected to the QT.")
            sys.stdout.flush()

            data = b""
            payload_size = struct.calcsize("L")  # 数据包的大小

            while True:
                while len(data) < payload_size:
                    packet = client_socket.recv(4096)
                    if not packet:
                        break
                    data += packet

                if not packet:
                    break

                packed_msg_size = data[:payload_size]
                data = data[payload_size:]

                msg_size = struct.unpack("L", packed_msg_size)[0]

                while len(data) < msg_size:
                    data += client_socket.recv(4096)

                frame_data = data[:msg_size]
                data = data[msg_size:]

                # 解码图像
                encoded_frame = pickle.loads(frame_data)
                frame = cv2.imdecode(encoded_frame, cv2.IMREAD_COLOR)

                frame = cv2.resize(frame, (640, 480))
                # cv2.imshow("Video Stream", frame)

                ##########################深度处理#############################
                # 使用YOLO模型进行推理（检测物体）
                results = model(frame)  # 直接使用frame输入

                # 每秒输出一次识别结果
                current_time = time.time()
                if current_time - last_time >= 1:  # 每秒输出一次
                    last_time = current_time
                    print(f"Detected {len(results[0].boxes)} objects")  # 输出检测到的物体数量
                    sys.stdout.flush()

                # 提取检测结果：boxes, labels, and scores
                boxes = results[0].boxes  # 获取检测框（boxes）
                confidences = boxes.conf  # 获取每个框的置信度
                labels = boxes.cls  # 获取每个框的类标签（类别ID）

                # 获取类别名称
                class_names = results[0].names  # 获取所有类的名称

                # 绘制检测框和标签
                for box, conf, label in zip(boxes.xywh, confidences, labels):
                    x1, y1, w, h = box[:4]  # 获取框的位置和大小
                    x1, y1, w, h = int(x1 - w / 2), int(y1 - h / 2), int(w), int(h)  # 将xywh转换为xyxy

                    label_name = class_names[int(label)]  # 获取标签名称

                    # 绘制框和标签
                    cv2.rectangle(frame, (x1, y1), (x1 + w, y1 + h), (0, 255, 0), 2)
                    cv2.putText(frame, f"{label_name} {conf:.2f}", (x1, y1 - 10),
                                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 0, 0), 2)
                ##################################################################

                # 转发视频帧给Qt客户端
                # 将视频帧编码成 JPEG 格式，并转发到 Qt
                _, encoded_frame = cv2.imencode('.jpg', frame)
                img_bytes = encoded_frame.tobytes()
                # frame_data = pickle.dumps(encoded_frame)  # 使用 pickle 序列化数据,不能用因为发给QT解包不出来
                message_size = struct.pack("!I", len(img_bytes))  # 数据包大小
                # 发送数据包到 Qt 客户端
                client_socket_qt.sendall(message_size + img_bytes)

                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break

            client_socket.close()
            print("Connection closed by server. Reconnecting...")
            sys.stdout.flush()
            time.sleep(2)  # 等待2秒后重连

        except Exception as e:
            print(f"Error: {e}. Attempting to reconnect...")
            sys.stdout.flush()
            client_socket.close()
            time.sleep(2)  # 等待2秒后重连


if __name__ == "__main__":
    start_client(host, port);
