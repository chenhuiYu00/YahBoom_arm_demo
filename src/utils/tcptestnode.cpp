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

  QTcpSocket socket;
  bool connected = false;

  while (true) {
    if (!connected) {
      // 2. 创建TCP连接
      socket.connectToHost(QHostAddress::LocalHost,
                           8188); // 连接到本地（或者目标IP）
      if (socket.waitForConnected(5000)) {
        qDebug() << "Successfully connected to host.";
        connected = true;
      } else {
        qDebug() << "Failed to connect to host, retrying in 1 seconds...";
        QThread::sleep(1); // 等待 1 秒后重试
        continue;
      }
    }

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

    // 4. 在写数据之前检查连接状态
    if (socket.state() == QAbstractSocket::ConnectedState) {
      socket.write(byteArray);
      socket.waitForBytesWritten();
    } else {
      qDebug() << "Connection is closed, stopping sending data.";
      connected = false; // 断开连接，重新连接
    }

    // 控制发送帧的速率（例如，每秒发送25帧）
    QThread::msleep(40);
  }

  return QCoreApplication::exec();
}
