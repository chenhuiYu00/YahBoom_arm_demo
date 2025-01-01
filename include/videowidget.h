#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>

#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QWidget>
#include <QtNetwork>

namespace Ui {
class VideoWidget;
}

class BoundingBox {
public:
  BoundingBox(const QRect &rect, const QString &label, const QColor &color)
      : rect(rect), label(label), color(color) {}

  // 绘制矩形框和标签
  void draw(QPainter &painter, double scaleX, double scaleY) const;

private:
  QRect rect;    // 边框的位置
  QString label; // 物体类别的名称
  QColor color;  // 边框的颜色
};

class VideoWidget : public QWidget {
  Q_OBJECT

public:
  explicit VideoWidget(QWidget *parent = nullptr);
  ~VideoWidget();

  QAbstractSocket::SocketState getSocketState() { return tcpSocket->state(); }

private:
  Ui::VideoWidget *ui;
  int videoIP = 8187;    // 用于接受识别结果的IP
  QTcpSocket *tcpSocket; // TCP连接
  QTcpServer *server;
  QTimer *frameTimer;         // 定时器，用于处理视频帧更新
  QByteArray videoDataBuffer; // 存储接收到的完整视频帧数据

  void onNewConnection();
  void updateVideoFrame(const QImage &image); // 更新视频帧的处理函数
  void drawBoundingBoxes(QPixmap &pixmap, double scaleX,
                         double scaleY); // 绘制识别框

private slots:
  void onReadyRead(); // 接收数据的槽函数
  void onTimeout();   // 处理视频帧超时，定时器回调
};

#endif // VIDEOWIDGET_H
