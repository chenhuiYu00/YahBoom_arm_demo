#include "include/videowidget.h"
#include "ui_videowidget.h"

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::VideoWidget), tcpSocket(new QTcpSocket(this)),
      server(new QTcpServer(this)), frameTimer(new QTimer(this)),
      rateTimer(new QTimer(this)) {
  ui->setupUi(this);

  // 启动TCP服务器，监听端口
  if (!server->listen(QHostAddress::LocalHost, videoIP)) {
    qDebug() << "Failed to start server!";
    return;
  }

  qDebug() << "Server started, waiting for connection...";

  // 连接新的客户端连接请求
  connect(server, &QTcpServer::newConnection, this,
          &VideoWidget::onNewConnection);

  // 设置定时器，定时刷新视频帧
  frameTimer->start(30); // 每30毫秒更新一次视频帧
  connect(frameTimer, &QTimer::timeout, this, &VideoWidget::onTimeout);

  initKBS();
}

VideoWidget::~VideoWidget() {
  if (tcpSocket) {
    tcpSocket->disconnectFromHost();
    tcpSocket->waitForDisconnected();
    qDebug() << "Tcp socket closed";
  }
  server->close();
  delete ui;
}

void VideoWidget::initKBS() {
  lastTime = QDateTime::currentMSecsSinceEpoch();
  // 初始化定时器，每秒更新码率
  connect(rateTimer, &QTimer::timeout, this, [=]() {
    // 每秒计算一次码率
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 elapsedTime = currentTime - lastTime;

    // 如果时间差大于1秒，计算码率
    if (elapsedTime > 1000) {
      qint64 bytesPerSecond =
          totalReceivedBytes / (elapsedTime / 1000.0); // 计算每秒字节数
      kbs = bytesPerSecond / 1024.0;                   // 转为 KBps

      // 重置时间和字节计数
      lastTime = currentTime;
      totalReceivedBytes = 0;
    }
  });

  rateTimer->start(1000); // 每秒更新一次码率
}

void VideoWidget::onNewConnection() {
  // 获取新的连接（客户端）
  tcpSocket = server->nextPendingConnection();
  qDebug() << "Client connected";

  // 连接数据接收槽
  connect(tcpSocket, &QTcpSocket::readyRead, this, &VideoWidget::onReadyRead);
}

void VideoWidget::onReadyRead() {
  static QByteArray data;
  static quint32 frameSize = 0;

  if (!tcpSocket) {
    qDebug() << "False tcp, return";
    return;
  }

  QByteArray rec = tcpSocket->readAll();
  data.append(rec);
  // 更新总接收字节数
  totalReceivedBytes += rec.size();

  while (data.size() >= sizeof(quint32)) {
    if (frameSize == 0) {
      // 读取消息的大小
      QDataStream stream(data);
      stream >> frameSize;
      data.remove(0, sizeof(quint32));
    }
    // qDebug() << frameSize;

    if (data.size() >= frameSize) {
      // 完整的数据包，提取并显示
      QByteArray frameData = data.left(static_cast<int>(frameSize));
      data.remove(0, static_cast<int>(frameSize));

      // 将图像数据加载为 QImage 并显示
      QImage image;
      if (image.loadFromData(frameData)) {
        // 视频帧更新
        updateVideoFrame(image);
      } else {
        qDebug() << "Unpack img fail";
      }
      frameSize = 0;
    } else {
      break; // 等待更多数据
    }
  }
}

void VideoWidget::updateVideoFrame(const QImage &image) {
  // 处理接收到的QImage图像
  QPixmap pixmap = QPixmap::fromImage(image);

  // 如果你想保持图像的比例，可以在显示时进行缩放
  QPixmap scaledPixmap = pixmap.scaled(ui->label->size(), Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

  // 获取缩放比例
  double scaleX = (double)scaledPixmap.width() / pixmap.width();
  double scaleY = (double)scaledPixmap.height() / pixmap.height();

  // 绘制边框
  // drawBoundingBoxes(scaledPixmap, scaleX, scaleY);
  // todo:也许边框可以预先在python处理

  // 设置视频帧到标签
  ui->label->setPixmap(scaledPixmap);
}

void VideoWidget::drawBoundingBoxes(QPixmap &pixmap, double scaleX,
                                    double scaleY) {
  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing); // 使绘制更加平滑

  // 定义边框信息，可以通过物体检测模型得到
  QList<BoundingBox> boundingBoxes;
  boundingBoxes.append(BoundingBox(QRect(50, 50, 100, 100), "Car", Qt::red));
  boundingBoxes.append(
      BoundingBox(QRect(200, 150, 120, 120), "Person", Qt::blue));
  boundingBoxes.append(BoundingBox(QRect(350, 200, 80, 80), "Tree", Qt::green));

  // 遍历并绘制每个边框
  for (const BoundingBox &box : boundingBoxes) {
    box.draw(painter, scaleX, scaleY); // 调用 BoundingBox 的 draw 方法
  }

  painter.end(); // 结束绘制
}

void VideoWidget::onTimeout() {
  // 这里你可以处理帧更新，或者定时处理其他任务
}

void BoundingBox::draw(QPainter &painter, double scaleX, double scaleY) const {

  // 缩放矩形的坐标和大小
  QRect scaledRect(rect.x() * scaleX, rect.y() * scaleY, rect.width() * scaleX,
                   rect.height() * scaleY);

  // 设置绘制矩形的样式
  painter.setPen(QPen(color, 2)); // 边框颜色和宽度
  painter.setBrush(QBrush(Qt::transparent));

  // 绘制矩形框
  painter.drawRect(scaledRect);

  // 绘制文本
  QPoint textPos(scaledRect.left() + 4, scaledRect.bottom() - 4);
  painter.setPen(QPen(color)); // 设置文本颜色
  painter.drawText(textPos, label);
}