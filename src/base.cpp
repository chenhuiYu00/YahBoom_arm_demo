#include "include/base.h"
#include "./ui_base.h"

base::base(QWidget *parent) : QMainWindow(parent), ui(new Ui::base) {
  ui->setupUi(this);

  // 初始化定时器，每500毫秒调用一次 onTimeout() 函数
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &base::onTimeout);
  timer->start(50); // 每50毫秒触发一次

  // 创建 TCP 套接字并连接到远程主机（替换为目标主机的 IP 和端口）
  socket.connectToHost(QHostAddress::LocalHost,
                       8189); // 连接到本地（或远程机器的 IP）
  if (!socket.waitForConnected()) {
    qDebug() << "Failed to connect to host!";
  }
}

base::~base() {
  delete ui;
  socket.close(); // 关闭连接
}

void base::sendCommand() {
  // 获取所有滑块的值并封装到字节数组中
  QByteArray byteArray;

  // 获取滑块的值并转换为字节
  for (int i = 0; i < 6; ++i) {
    auto *slider = findChild<QSlider *>(QString("horizontalSlider_%1").arg(i));
    if (slider) {
      int angle = slider->value(); // 获取当前滑块的值，范围 0-180
      byteArray.append(
          static_cast<char>(angle)); // 将角度值作为字节添加到字节数组
    }
  }

  // 发送数据到远程设备
  socket.write(byteArray);
  socket.waitForBytesWritten(); // 等待直到数据被写入
  qDebug() << "Data sent: " << byteArray.toHex();
}

void base::onTimeout() { sendCommand(); }
