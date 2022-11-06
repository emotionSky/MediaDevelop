#include "ffwindow.h"
#include "ui_ffwindow.h"

#include <QDebug>
#include <QFile>

extern "C" {
    // 设备
#include <libavdevice/avdevice.h>
    // 格式
#include <libavformat/avformat.h>
    // 工具（比如错误处理）
#include <libavutil/avutil.h>
    // 编码
#include <libavcodec/avcodec.h>
}

#ifdef Q_OS_WIN
constexpr char FMT_NAME[] = "dshow";
#else
constexpr char FMT_NAME[] = "avfoundation";
#endif

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
}

FfWindow::~FfWindow()
{
    delete ui;
}


void FfWindow::on_audioButton_clicked()
{
    // 获取输入格式对象
    const AVInputFormat *pFmt = av_find_input_format(FMT_NAME);
    if (!pFmt)
    {
        qDebug() << "获取输入格式对象失败，fmt=" << FMT_NAME;
        return;
    }

    // 上下文
    AVFormatContext* pCtx = nullptr;
    // 设备名称
#ifdef Q_OS_WIN
    const char* devName = "audio=Microphone (High Definition Audio Device)";
#else
    const char* devName = ":0";
#endif
    // 选项，暂时用不到
    // AVDictionary* pOptions = nullptr;

    // 打开设备
    char errBuf[1024] = {0};
    int ret = avformat_open_input(&pCtx, devName, pFmt, nullptr);
    if (ret < 0)
    {
        av_strerror(ret, errBuf, sizeof(errBuf));
        qDebug() << "打开设备失败，错误信息：" << errBuf;
        return;
    }

    /* 文件操作 */
#ifdef Q_OS_WIN
    const char* fileName = "E:/out.pcm";
#else
    const char* fileName = "/home/out.pcm";
#endif
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "文件打开失败";

        // 关闭设备
        avformat_close_input(&pCtx);
        return;
    }

    // 控制采集次数
    int count = 50;

    // 采集数据
    // 数据包
    AVPacket pkt;
    //采集数据
    while (count-- > 0 && av_read_frame(pCtx, &pkt) == 0)
    {
        file.write((const char*)pkt.data, pkt.size);
    }

    file.close();
    avformat_close_input(&pCtx);
}



