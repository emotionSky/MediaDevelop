#include "ffwindow.h"
#include "ui_ffwindow.h"

#include <QDebug>

FfWindow::FfWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FfWindow)
{
    ui->setupUi(this);
    //qDebug() << av_version_info();

#if 0
    //这里使用局部变量，无法进行内存释放，如果需要，使用成员变量即可
    QPushButton* pBtn = new QPushButton(this);
    pBtn->setText("关闭");
    pBtn->setFixedSize(100, 30);
    //pBtn->setParent(this); //这个可以直接在构造的时候实现

    connect(pBtn, &QPushButton::clicked, this, &FfWindow::close);
    //使用 disconnect 可以取消信号与槽的连接
#endif

    m_pAudioRecorder = new RecordAudioThread(); //这里不能挂在this，否则无法传入到线程中
    m_pRecordThd = new QThread(this);
    m_pAudioRecorder->moveToThread(m_pRecordThd);

    connect(this, &FfWindow::RunRecordSignal, m_pAudioRecorder, &RecordAudioThread::Run);
    connect(this, &QThread::destroyed, this, &FfWindow::OnDestroyRecord);
}

FfWindow::~FfWindow()
{
    delete ui;
}

void FfWindow::OnDestroyRecord()
{
    if (m_pRecordThd->isRunning())
    {
        m_pAudioRecorder->Stop();
        m_pRecordThd->quit();  ///<退出线程
        m_pRecordThd->wait();  ///<退出线程之后必须调用这个函数，进行一些数据的清理
    }

    /* 注意需要在这里进行内存释放！ */
    if (m_pAudioRecorder)
    {
        delete m_pAudioRecorder;
        m_pAudioRecorder = nullptr;
    }
}

void FfWindow::on_audioButton_clicked()
{
    if (m_pRecordThd->isRunning())
    {
        ui->audioButton->setText("开启录音");
        m_pAudioRecorder->Stop(); ///<设置标志位
        m_pRecordThd->quit();     ///<退出线程
        m_pRecordThd->wait();     ///<退出线程之后必须调用这个函数，进行一些数据的清理
    }
    else
    {
        ui->audioButton->setText("结束录音");
        m_pAudioRecorder->Start();///<设置了线程函数运行的标志位，还是没有启动线程函数
        m_pRecordThd->start();    ///<启动了线程，但是还没有启动线程函数
        emit RunRecordSignal();   ///<通过信号和槽的机制启动线程函数
    }
}
