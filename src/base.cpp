#include "include/base.h"
#include "./ui_base.h"

base::base(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::base), server(new QTcpServer(this)) {
  ui->setupUi(this);

  // 初始化定时器，每50毫秒调用一次 onTimeout() 函数
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &base::onTimeout);
  timer->start(50); // 每50毫秒触发一次

  // 启动TCP服务器，监听端口
  if (!server->listen(QHostAddress::Any, commandPort)) {
    qDebug() << "Failed to start server!";
    return;
  }
  qDebug() << "Server started, waiting for connection...";
  qDebug() << "local server ip: " << server->serverAddress().toString();
  // 连接新的客户端连接请求
  connect(server, &QTcpServer::newConnection, this,
          &base::onNewControlConnection);

  initStatusBar();
  initMenuBar();
  initPython();
}

base::~base() {
  delete ui;
  socket->close(); // 关闭连接
}

void base::onNewControlConnection() {
  // 获取新的连接（客户端）
  socket = server->nextPendingConnection();
  qDebug() << "Command client connected: " << socket->peerAddress().toString();
  ui->textBrowser_PyOut->append("Remote ip: " +
                                socket->peerAddress().toString());

  updateConnectionStatus(statusCircleLabel, true); // 更新为已连接
  readySend = true;
  // 创建 TCP 套接字并连接信号槽
  connect(socket, &QTcpSocket::disconnected, this, [=]() {
    updateConnectionStatus(statusCircleLabel, false); // 更新为未连接
    readySend = false;
  });
}

void base::initPython() {
  pythonThread =
      new PythonThread("/home/yuchen/Fdisk/Projects/YahBoom/YahBoom/python/"
                       "detection.py",
                       *ui->textBrowser_PyOut, this);
  connect(ui->pushButton_startPy, &QPushButton::clicked, this,
          [=]() { emit pythonThread->startPython(); });
  connect(ui->pushButton_closePy, &QPushButton::clicked, this,
          [=]() { emit pythonThread->shutDownPython(); });

  pythonThread->start();
}

void base::initStatusBar() { // 创建 QLabel 来显示连接状态的圆圈和文字
  statusCircleLabel = new QLabel(this);
  statusTextLabel = new QLabel("指令端", this);
  videoStatusCircleLabel = new QLabel(this);
  videoStatusTextLabel = new QLabel("视频", this);
  statusKBSLabel = new QLabel(this);
  statusKBSLabel->setText("Bitrate: 0 KBps"); // 初始显示0

  QWidget *spacer = new QWidget(this), *spacer2 = new QWidget(this),
          *spacer3 = new QWidget(this); // 创建一个 spacer，用来添加左侧空白间距
  spacer->setFixedWidth(6); // 设置空白区域的宽度
  spacer2->setFixedWidth(6);
  spacer3->setFixedWidth(6);

  // 设置圆圈标签的大小
  statusCircleLabel->setFixedSize(20, 20);
  videoStatusCircleLabel->setFixedSize(20, 20);
  statusCircleLabel->setStyleSheet(
      "background-color: red; border-radius: 10px;");
  videoStatusCircleLabel->setStyleSheet(
      "background-color: red; border-radius: 10px;");

  // 将 spacer、圆圈和文字添加到 statusBar
  statusBar()->addWidget(spacer); // 添加空白区域
  statusBar()->addWidget(statusCircleLabel);
  statusBar()->addWidget(statusTextLabel);
  statusBar()->addWidget(spacer2);
  statusBar()->addWidget(videoStatusCircleLabel);
  statusBar()->addWidget(videoStatusTextLabel);
  statusBar()->addWidget(spacer3);
  statusBar()->addWidget(statusKBSLabel);
}

void base::initMenuBar() {
  connect(ui->action_control, &QAction::triggered, this, [=]() {
    recentModel = 0;
    updateMenuBar();
  });
  connect(ui->action_detection, &QAction::triggered, this, [=]() {
    recentModel = 1;
    updateMenuBar();
  });

  updateMenuBar();
}

void base::updateMenuBar() {
  if (recentModel)
    ui->recentModel->setTitle("当前模式：识别");
  else
    ui->recentModel->setTitle("当前模式：直控");
}

void base::updateConnectionStatus(QLabel *circle, bool connected) {
  // 根据连接状态更新圆圈颜色和状态文本
  if (connected) {
    circle->setStyleSheet("background-color: green; border-radius: 10px;");
  } else {
    circle->setStyleSheet("background-color: red; border-radius: 10px;");
  }
}

void base::sendCommand() {
  // 获取所有滑块的值并封装到字节数组中
  QByteArray byteArray;

  detection = ui->widget_video->getDetect();
  byteArray.append(static_cast<char>(recentModel));
  byteArray.append(static_cast<char>(detection));

  // 获取滑块的值并转换为字节
  for (int i = 0; i < 6; ++i) {
    auto *slider = findChild<QSlider *>(QString("horizontalSlider_%1").arg(i));
    int sliderValue = slider->value(); // 获取滑块的值，范围 0-99
    int angle = static_cast<int>((sliderValue / 99.0) * 180); // 映射到 0-180
    byteArray.append(
        static_cast<char>(angle)); // 将角度值作为字节添加到字节数组
  }

  // 发送数据到远程设备
  socket->write(byteArray);
  socket->waitForBytesWritten(); // 等待直到数据被写入
  // qDebug() << "Data sent: " << byteArray.toHex();
}

void base::onTimeout() {
  // 更新视频状态图标
  updateConnectionStatus(videoStatusCircleLabel,
                         ui->widget_video->getSocketState() ==
                             QAbstractSocket::ConnectedState);
  // 更新传输速率
  statusKBSLabel->setText(
      QString("ByteRate: %1 KB/s").arg(ui->widget_video->getKBS()));

  // 更新滑块数值
  for (int i = 0; i < 6; ++i) {
    auto *slider = findChild<QSlider *>(QString("horizontalSlider_%1").arg(i));
    auto *label = findChild<QLabel *>(QString("label_%1").arg(i));

    if (slider && label) {
      int sliderValue = slider->value(); // 获取滑块的值，范围 0-99
      int angle = static_cast<int>((sliderValue / 99.0) * 180); // 映射到 0-180
      // 显示在label上，格式为 "序号: 角度" (格式对齐)
      label->setText(QString("%1: %2").arg(i).arg(
          angle, 3, 10, QChar('0'))); // 角度固定宽度3位数，0填充
    }
  }

  if (readySend)
    sendCommand();
}
