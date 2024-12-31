#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>

#include <QImage>
#include <QMessageBox>
#include <QString>
#include <QTcpSocket>
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
};

#endif // VIDEOWIDGET_H
