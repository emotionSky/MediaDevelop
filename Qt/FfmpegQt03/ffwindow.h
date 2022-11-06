#ifndef FFWINDOW_H
#define FFWINDOW_H

#include <QMainWindow>
#include <thread>
#include "recordaudiothread.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class FfWindow;
}
QT_END_NAMESPACE

class FfWindow : public QMainWindow
{
    Q_OBJECT

public:
    FfWindow(QWidget *parent = nullptr);
    ~FfWindow();

signals:
    void RunRecordSignal();

private slots:
    void OnThreadFinished();
    void on_audioButton_clicked();

private:
    Ui::FfWindow *ui;
    RecordAudioThread* m_pAudioRecorder;
    std::thread* m_pThread;
};
#endif // FFWINDOW_H
