#include "include/pythonThread.h"
#include <QPushButton>

PythonThread::PythonThread(const QString &scriptPath, QTextBrowser &b,
                           QObject *parent)
    : QThread(parent), browser(b), scriptPath(scriptPath),
      process(new QProcess()) {
  connect(this, &PythonThread::startPython, this, [=]() { onStartScript(); });
  connect(this, &PythonThread::shutDownPython, this, [=]() { onStopScript(); });

  browser.append("Clicking button to start python script.");
}

void PythonThread::run() {
  exec(); // 启动事件循环，使 QProcess 在线程中工作
}

PythonThread::~PythonThread() {
  if (process->state() == QProcess::Running) {
    process->terminate();           // 尝试终止进程
    process->waitForFinished(3000); // 等待脚本退出
  }
}

void PythonThread::onStopScript() {
  if (process->state() == QProcess::Running) {
    process->terminate();           // 尝试终止进程
    process->waitForFinished(3000); // 等待脚本退出
  }
  qDebug() << "Python script stopped";
  browser.append("Python script stopped");
}

void PythonThread::onStartScript() {
  if (process->state() == QProcess::Running) {
    browser.append("Python script already running");
    return;
  }

  // 设置 Python 解释器的路径
  QString pythonPath =
      "/home/yuchen/Fdisk/miniconda3/envs/pytorch/bin/python"; // 修改为实际路径

  // 设置运行的命令和参数
  QStringList arguments;
  arguments << scriptPath;

  // 启动 Python 脚本
  process->start(pythonPath, arguments);

  // 捕获输出
  connect(process, &QProcess::readyReadStandardOutput, [=]() {
    QString output = process->readAllStandardOutput();
    // 去除多余的换行符
    output.replace(QRegExp("[\r\n]+"), " ");
    output = output.trimmed(); // 去除开头和结尾的空格
    qDebug() << output;
    browser.append(output); // 将标准输出追加到 QTextBrowser
  });
  connect(process, &QProcess::readyReadStandardError, [=]() {
    QString errorOutput = process->readAllStandardError();
    // 去除多余的换行符
    errorOutput.replace(QRegExp("[\r\n]+"), " ");
    errorOutput = errorOutput.trimmed(); // 去除开头和结尾的空格
    qDebug() << errorOutput;
    browser.append(errorOutput); // 将标准错误追加到 QTextBrowser
  });

  // 检查脚本是否成功启动
  if (!process->waitForStarted()) {
    qDebug() << "Failed to start Python script";
    browser.append("Failed to start Python script");
  } else {
    qDebug() << "Python script is running...";
    browser.append("Python script is running...");
  }
}
