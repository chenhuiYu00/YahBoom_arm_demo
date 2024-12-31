#include "include/base.h"
#include "./ui_base.h"

base::base(QWidget *parent) : QMainWindow(parent), ui(new Ui::base) {
  ui->setupUi(this);

  // 初始化定时器，每500毫秒调用一次 onTimeout() 函数
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &base::onTimeout);
  timer->start(50); // 每50毫秒触发一次

  initStatusBar();

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

void base::initStatusBar() { // 创建 QLabel 来显示连接状态的圆圈和文字
  statusCircleLabel = new QLabel(this);
  statusTextLabel = new QLabel("未连接", this);
  QWidget *spacer = new QWidget(this); // 创建一个 spacer，用来添加左侧空白间距
  spacer->setFixedWidth(6); // 设置空白区域的宽度

  // 设置圆圈标签的大小
  statusCircleLabel->setFixedSize(20, 20);
  statusCircleLabel->setStyleSheet(
      "background-color: red; border-radius: 10px;");

  // 将 spacer、圆圈和文字添加到 statusBar
  statusBar()->addWidget(spacer); // 添加空白区域
  statusBar()->addWidget(statusCircleLabel);
  statusBar()->addWidget(statusTextLabel);

  // 创建 TCP 套接字并连接信号槽
  connect(&socket, &QTcpSocket::connected, this, [=]() {
    updateConnectionStatus(true); // 更新为已连接
  });
  connect(&socket, &QTcpSocket::disconnected, this, [=]() {
    updateConnectionStatus(false); // 更新为未连接
  });
}

void base::updateConnectionStatus(bool connected) {
  // 根据连接状态更新圆圈颜色和状态文本
  if (connected) {
    statusCircleLabel->setStyleSheet(
        "background-color: green; border-radius: 10px;");
    statusTextLabel->setText("已连接");
  } else {
    statusCircleLabel->setStyleSheet(
        "background-color: red; border-radius: 10px;");
    statusTextLabel->setText("未连接");
  }
}

void base::sendCommand() {
  // 获取所有滑块的值并封装到字节数组中
  QByteArray byteArray;

  // 获取滑块的值并转换为字节
  for (int i = 0; i < 6; ++i) {
    auto *slider = findChild<QSlider *>(QString("horizontalSlider_%1").arg(i));
    auto *label = findChild<QLabel *>(QString("label_%1").arg(i));

    if (slider && label) {
      int sliderValue = slider->value(); // 获取滑块的值，范围 0-99
      int angle = static_cast<int>((sliderValue / 99.0) * 180); // 映射到 0-180
      // 显示在label上，格式为 "序号: 角度" (格式对齐)
      label->setText(QString("%1: %2").arg(i).arg(
          angle, 3, 10, QChar('0'))); // 角度固定宽度3位数，0填充

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
