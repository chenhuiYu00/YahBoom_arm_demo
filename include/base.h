#ifndef BASE_H
#define BASE_H

#include <QMainWindow>

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

private:
  Ui::base *ui;
};
#endif // BASE_H
