#include <QByteArray>
#include <QCoreApplication>
#include <QDataStream>
#include <QTcpSocket>
#include <QThread>
#include <QtNetwork>
#include <opencv4/opencv2/opencv.hpp>

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  // 1. 打开本地摄像头
  cv::VideoCapture cap(0); // 使用默认摄像头
  if (!cap.isOpened()) {
    qDebug() << "Failed to open camera!";
    return -1;
  }

  // 2. 创建TCP连接
  QTcpSocket socket;
  socket.connectToHost(QHostAddress::LocalHost,
                       8188); // 连接到本地（或者目标IP）
  if (!socket.waitForConnected()) {
    qDebug() << "Failed to connect to host!";
    return -1;
  }

  while (true) {
    cv::Mat frame;
    cap >> frame; // 捕获一帧

    if (frame.empty()) {
      break; // 如果帧为空，则退出
    }

    // 3. 将帧编码为 JPEG
    std::vector<uchar> buf;
    cv::imencode(".jpg", frame, buf);
    QByteArray byteArray(reinterpret_cast<const char *>(buf.data()),
                         buf.size());

    // 4. 发送帧大小和帧数据
    socket.write(byteArray);
    socket.waitForBytesWritten();

    // 控制发送帧的速率（例如，每秒发送25帧）
    QThread::msleep(40);
  }

  return a.exec();
}
