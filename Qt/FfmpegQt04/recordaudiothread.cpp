#include "recordaudiothread.h"
#include <QFile>
#include <QTime>
#include <QtDebug>

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
constexpr char FILE_PATH[] = "E:/record/";
#else
constexpr char FMT_NAME[] = "avfoundation";
constexpr char FILE_PATH[] = "/home/record/";
#endif

RecordAudioThread::RecordAudioThread(QObject *parent) : QObject(parent),
    m_bRunning(false)
{

}

RecordAudioThread::~RecordAudioThread()
{
    Stop();
}

void RecordAudioThread::Start()
{
    m_bRunning = true;
}

void RecordAudioThread::Stop()
{
    m_bRunning = false;
}

void ShowSpec(AVFormatContext* pCtx)
{
    AVStream* stream = pCtx->streams[0];
    AVCodecParameters* params = stream->codecpar;
    qDebug() << "channel=" << params->channels;
    qDebug() << "sample_rate=" << params->sample_rate;
    qDebug() << "format=" << params->format;

    //每一个样本的一个声道占用多少个字节
    qDebug() << "bytes_per_ch=" << av_get_bytes_per_sample((AVSampleFormat)params->format);
}

void RecordAudioThread::Run()
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

    // 打开设备
    char errBuf[1024] = {0};
    int ret = avformat_open_input(&pCtx, devName, pFmt, nullptr);
    if (ret < 0)
    {
        av_strerror(ret, errBuf, sizeof(errBuf));
        qDebug() << "打开设备失败，错误信息：" << errBuf;
        emit error_signal();
        return;
    }

    /* 文件操作 */
    QString fileName = QString(FILE_PATH) + QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss") + ".pcm";
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "文件打开失败";

        // 关闭设备
        avformat_close_input(&pCtx);
        emit error_signal();
        return;
    }

    /* 采集数据 */
    AVPacket pkt;  // 数据包
    int res = 0;
    while (m_bRunning)
    {
        res = av_read_frame(pCtx, &pkt);
        if (ret == 0)
        {
            file.write((const char*)pkt.data, pkt.size);
        }
#if 0
        else if (ret == AVERROR(EAGAIN))
        {
            char errBuf[1024];
            av_strerror(ret, errBuf, sizeof(errBuf));
            qDebug("err=%d,msg=%s", res, errBuf);
            break;
        }
#endif
    }

    file.close();
    avformat_close_input(&pCtx);
}
