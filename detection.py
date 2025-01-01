import socket
import cv2
import sys
import numpy as np
import pickle
import time
import torch
import atexit

def main():
    print(f"Using torch version: {torch.__version__}")
    sys.stdout.flush() 
    
    qtPort=8187
    videoIP="127.0.0.1"
    videoPort=8188

    # 创建 TCP 客户端套接字（连接到 Qt 程序）
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect(("localhost", qtPort))  # 本地端口连接Qt程序

    # 创建视频接收的 TCP 服务器套接字（接收远程视频流）
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # 绑定之前设置 SO_REUSEADDR
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # 在程序结束时，自动调用关闭 socket 的方法
    atexit.register(lambda: server_socket.close())

    # 尝试连接并接收视频流
    while True:
        try:
            # 尝试绑定并监听连接
            server_socket.bind((videoIP, videoPort))
            server_socket.listen(1)
            print("Waiting for video connection...")
            sys.stdout.flush() 

            # 等待远程视频流连接
            video_socket, addr = server_socket.accept()
            print(f"Connection established with {addr}")
            sys.stdout.flush() 
            
            # 接收视频流并进行处理
            while True:
                data = b""
                while len(data) < 88888:
                    packet = video_socket.recv(88888)
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
                    client_socket.sendall(img_bytes)
                except Exception as e:
                    print(f"Error sending image to Qt: {e}")
                    sys.stdout.flush() 
                    break  # 如果发送失败，退出当前循环并重新尝试连接

        except (socket.error, Exception) as e:
            print(f"Error occurred: {e}")
            print("Reattempting to bind and accept connection...")
            sys.stdout.flush() 
            server_socket.close()  # 关闭现有连接，准备重新绑定
            time.sleep(3)  # 延时3秒再进行重试
            server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 重新创建socket
            server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

if __name__ == "__main__":
    main()
