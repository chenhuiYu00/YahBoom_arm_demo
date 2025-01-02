import socket
import sys
import time

import cv2
import numpy as np
import torch


def main():
    print(f"Using torch version: {torch.__version__}")
    sys.stdout.flush()

    qtPort = 8187
    videoIP = "192.168.1.11"
    videoPort = 9995

    # 创建 TCP 客户端套接字（连接到 Qt 程序）
    client_socket_qt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket_qt.connect(("localhost", qtPort))  # 本地端口连接 Qt 程序

    while True:
        try:
            # 创建 TCP 客户端套接字（连接到发送端）
            client_socket_video = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket_video.connect((videoIP, videoPort))  # 主动连接发送端
            print(f"Connected to video server at {videoIP}:{videoPort}")
            sys.stdout.flush()

            while True:
                data = b""
                while len(data) < 88888:
                    packet = client_socket_video.recv(88888)
                    if not packet:
                        break
                    data += packet

                if not data:  # 如果没有接收到数据，表示连接已经断开
                    print("Connection lost, retrying...")
                    sys.stdout.flush()
                    break  # 跳出当前接收循环，重新尝试连接

                # 解码图像
                img = np.frombuffer(data, dtype=np.uint8)
                frame = cv2.imdecode(img, cv2.IMREAD_COLOR)

                # 在这里处理图像，例如深度学习识别或其他操作
                # 进行深度学习处理，假设使用 OpenCV 或深度学习框架进行处理
                '''
                # 假设识别结果是一个框，识别出物体并标记
                boxes = [(50, 50, 100, 100, "Object1"), (150, 150, 120, 120, "Object2")]
                for box in boxes:
                    x, y, w, h, label = box
                    cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)  # 绘制框
                    cv2.putText(frame, label, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)
                '''

                # 编码图像并通过 Qt 发送
                _, img_encoded = cv2.imencode('.jpg', frame)
                img_bytes = img_encoded.tobytes()
                try:
                    client_socket_qt.sendall(img_bytes)
                except Exception as e:
                    print(f"Error sending image to Qt: {e}")
                    sys.stdout.flush()
                    break  # 如果发送失败，退出当前循环并重新尝试连接

        except (socket.error, Exception) as e:
            print(f"Error occurred: {e}")
            print("Retrying connection to video server...")
            sys.stdout.flush()
            time.sleep(3)


if __name__ == "__main__":
    main()
