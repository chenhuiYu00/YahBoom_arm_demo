#include "include/videowidget.h"
#include "ui_videowidget.h"

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::VideoWidget), tcpSocket(new QTcpSocket(this)),
      videoLabel(new QLabel(this)), frameTimer(new QTimer(this)) {
  ui->setupUi(this);

  // 连接TCP
  tcpSocket->connectToHost(QHostAddress::LocalHost,
                           8188); // 替换为实际IP和端口
                                  // 本地：QHostAddress::LocalHost
  connect(tcpSocket, &QTcpSocket::readyRead, this, &VideoWidget::onReadyRead);

  // 设置定时器，定时刷新视频帧
  frameTimer->start(30); // 每30毫秒更新一次视频帧
  connect(frameTimer, &QTimer::timeout, this, &VideoWidget::onTimeout);
}

VideoWidget::~VideoWidget() { delete ui; }

void VideoWidget::onReadyRead() {
  // 读取数据并保存到缓存中
  QByteArray data = tcpSocket->readAll();
  videoDataBuffer.append(data);

  if (data.size() < 128) {
    qDebug() << "Received invalid frame size: " << data.size();
    return; // 丢弃数据

    // 假设视频数据已经是完整的一帧，进行解码（此处简单示例，实际可能需要根据协议进行拆包）
    // 例如解码JPEG帧
    QImage img;
    if (img.loadFromData(videoDataBuffer)) {
      // 视频帧更新
      updateVideoFrame(img);
      videoDataBuffer.clear(); // 清空缓存，准备接收下一帧
    }
  }
}

void VideoWidget::updateVideoFrame(const QImage &image) {
  // 处理接收到的QImage图像
  QPixmap pixmap = QPixmap::fromImage(image);

  // 如果你想保持图像的比例，可以在显示时进行缩放
  QPixmap scaledPixmap = pixmap.scaled(videoLabel->size(), Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

  // 设置视频帧到标签
  videoLabel->setPixmap(scaledPixmap);
}

void VideoWidget::drawBoundingBoxes(QImage &image) {
  QPainter painter(&image);
  painter.setPen(QPen(Qt::red, 2)); // 设置红色边框和宽度
  painter.setBrush(QBrush(Qt::transparent));

  // 假设你有检测到的物体的边框坐标，以下是一个简单的例子
  QRect rect(50, 50, 100, 100); // 物体边框坐标（示例）
  painter.drawRect(rect);       // 绘制矩形框

  // 如果有多个边框，重复绘制
  // QRect rect2(200, 150, 120, 120);
  // painter.drawRect(rect2);
}

void VideoWidget::onTimeout() {
  // 这里你可以处理帧更新，或者定时处理其他任务
}