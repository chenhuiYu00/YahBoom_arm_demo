#ifndef BASE_H
#define BASE_H

#include <QByteArray>
#include <QDebug>
#include <QLabel>
#include <QMainWindow>
#include <QPainter>
#include <QPixmap>
#include <QStatusBar>
#include <QTcpSocket>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class base;
}
QT_END_NAMESPACE

class base : public QMainWindow {
  Q_OBJECT

public:
  base(QWidget *parent = nullptr);
  ~base();

private slots:
  void onTimeout();                            // 定时器超时的槽函数
  void updateConnectionStatus(bool connected); // 更新连接状态的槽函数

private:
  Ui::base *ui;
  int commandIP = 8189;      // 用于发送控制指令的IP
  QTcpSocket socket;         // 用于与远程设备通信的 TCP 套接字
  QTimer *timer;             // 定时器，用于定期读取滑块值并发送
  QLabel *statusCircleLabel; // 用于显示连接状态圆圈的标签
  QLabel *statusTextLabel;   // 用于显示连接状态文字的标签

  void initStatusBar();
  void sendCommand();
};
#endif // BASE_H
