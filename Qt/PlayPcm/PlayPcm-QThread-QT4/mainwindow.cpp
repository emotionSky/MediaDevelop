#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&m_player, &QThread::finished,
            [this]()
    {
        ui->pushButton_2->setText("开始播放");
    });
}

MainWindow::~MainWindow()
{
    if (m_player.isRunning()) //停止播放
    {
        m_player.requestInterruption();
        m_player.quit();
        m_player.wait();
    }
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    m_player.PrintVersion();
}

void MainWindow::on_pushButton_2_clicked()
{
    if (m_player.isRunning()) //停止播放
    {
        m_player.requestInterruption();
        m_player.quit();
        m_player.wait();
    }
    else //开始播放
    {
        m_player.start();
        ui->pushButton_2->setText("停止播放");
    }
}
