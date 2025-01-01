#ifndef PYTHONTHREAD_H
#define PYTHONTHREAD_H

#include <QDebug>
#include <QProcess>
#include <QTextBrowser>
#include <QThread>

class PythonThread : public QThread {
  Q_OBJECT

public:
  explicit PythonThread(const QString &scriptPath, QTextBrowser &b,
                        QObject *parent = nullptr);

  void run() override;
  ~PythonThread() override;

private:
  QProcess *process{}; // 用于启动和管理 Python 脚本的进程
  QTextBrowser &browser;
  QString scriptPath;

  void onStartScript(); // 启动脚本
  void onStopScript();  // 停止脚本

signals:
  void shutDownPython();
  void startPython();
};

#endif // PYTHONTHREAD_H
