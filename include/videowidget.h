#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>

#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QTcpSocket>
#include <QTimer>
#include <QWidget>
#include <QtNetwork>

namespace Ui {
class VideoWidget;
}

class VideoWidget : public QWidget {
  Q_OBJECT

public:
  explicit VideoWidget(QWidget *parent = nullptr);
  ~VideoWidget();

private:
  Ui::VideoWidget *ui;
  QTcpSocket *tcpSocket;      // TCP连接
  QLabel *videoLabel;         // 显示视频帧的标签
  QTimer *frameTimer;         // 定时器，用于处理视频帧更新
  QByteArray videoDataBuffer; // 存储接收到的完整视频帧数据

  void updateVideoFrame(const QImage &image); // 更新视频帧的处理函数
  void drawBoundingBoxes(QImage &image);      // 绘制识别框

private slots:
  void onReadyRead(); // 接收数据的槽函数
  void onTimeout();   // 处理视频帧超时，定时器回调
};

#endif // VIDEOWIDGET_H
