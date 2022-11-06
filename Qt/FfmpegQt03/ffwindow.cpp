#include "ffwindow.h"
#include "ui_ffwindow.h"

#include <QDebug>

FfWindow::FfWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FfWindow)
{
    ui->setupUi(this);
    //qDebug() << av_version_info();

    m_pAudioRecorder = new RecordAudioThread(); //这里不能挂在this，否则无法传入到线程中
    connect(m_pAudioRecorder, &RecordAudioThread::error_signal, this, &FfWindow::OnThreadFinished);
}

FfWindow::~FfWindow()
{
    if (m_pAudioRecorder->IsRun())
    {
        m_pAudioRecorder->Stop();
        m_pThread->join();
        delete m_pThread;
        m_pThread = nullptr;
    }

    if (m_pAudioRecorder)
    {
        delete m_pAudioRecorder;
        m_pAudioRecorder = nullptr;
    }
    delete ui;
}

void FfWindow::OnThreadFinished()
{
    m_pThread->join();
    delete m_pThread;
    m_pThread = nullptr;
    ui->audioButton->setText("开始录音");
}

void FfWindow::on_audioButton_clicked()
{
    if (m_pAudioRecorder->IsRun())
    {
        ui->audioButton->setText("开启录音");
        m_pAudioRecorder->Stop(); ///<设置标志位
        m_pThread->join();
        delete m_pThread;
        m_pThread = nullptr;
    }
    else
    {
        ui->audioButton->setText("结束录音");
        m_pAudioRecorder->Start();///<设置了线程函数运行的标志位，还是没有启动线程函数
        m_pThread = new std::thread(&RecordAudioThread::Run, m_pAudioRecorder);
    }
}
