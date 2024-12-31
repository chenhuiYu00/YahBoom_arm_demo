#ifndef BASE_H
#define BASE_H

#include <QByteArray>
#include <QDebug>
#include <QMainWindow>
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
  void onTimeout(); // 定时器超时的槽函数

private:
  Ui::base *ui;
  QTcpSocket socket; // 用于与远程设备通信的 TCP 套接字
  QTimer *timer;     // 定时器，用于定期读取滑块值并发送

  void sendCommand();
};
#endif // BASE_H
