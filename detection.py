import pickle
import socket
import struct
import sys
import time

import cv2

host = '192.168.1.11'
port = 8188
qtPort = 8187


def start_client(host='127.0.0.1', port=12345):
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
                #cv2.imshow("Video Stream", frame)

                ######深度处理########

                #####################

                # 转发视频帧给Qt客户端
                # 将视频帧编码成 JPEG 格式，并转发到 Qt
                _, encoded_frame = cv2.imencode('.jpg', frame)
                img_bytes = encoded_frame.tobytes()
                #frame_data = pickle.dumps(encoded_frame)  # 使用 pickle 序列化数据,不能用因为发给QT解包不出来
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
