#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_player.moveToThread(&m_thread);
    connect(&m_thread, &QThread::started, &m_player, &PlayThread::Run);
    connect(&m_player, &PlayThread::ExistRun, this, &MainWindow::StopThread);
    //监听线程结束
    connect(&m_thread, &QThread::finished,
            [this]()
    {
        ui->pushButton_2->setText("开始播放");
    });
}

MainWindow::~MainWindow()
{
    if (m_thread.isRunning()) //停止播放
    {
        m_thread.requestInterruption();
        m_thread.quit();
        m_thread.wait();
        ui->pushButton_2->setText("开始播放");
    }
    delete ui;
}

void MainWindow::StopThread()
{
    m_player.SetRunning(false);
    m_thread.requestInterruption();
    m_thread.quit();
    m_thread.wait();
}

void MainWindow::on_pushButton_clicked()
{
    m_player.PrintVersion();
}

void MainWindow::on_pushButton_2_clicked()
{
    if (m_thread.isRunning()) //停止播放
    {
        StopThread();
    }
    else //开始播放
    {
        m_thread.start();
        ui->pushButton_2->setText("停止播放");
    }
}
