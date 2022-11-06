#include "recordaudiothread.h"
#include <QFile>
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
#else
constexpr char FMT_NAME[] = "avfoundation";
#endif

RecordAudioThread::RecordAudioThread(QObject *parent) : QObject(parent),
    m_bRunning(false)
{

}

void RecordAudioThread::Start()
{
    m_bRunning = true;
}

void RecordAudioThread::Stop()
{
    m_bRunning = false;
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

    // 采集数据
    // 数据包
    AVPacket pkt;
    //采集数据
#if 0
    while (m_bRunning && av_read_frame(pCtx, &pkt) == 0)
    {
        file.write((const char*)pkt.data, pkt.size);
    }
#endif
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
