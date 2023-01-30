#include "playthread.h"
#include <SDL.h>
#include <QDebug>
#include <QFile>

constexpr char FILE_NAME[] = "D:\\Git\\MediaDevelop\\Qt\\resource\\mom_s16le_ch2_44100.pcm";
constexpr int SAMPLE_RATE = 44100;
constexpr int FORMAT = AUDIO_S16; //
constexpr int SAMPLE_SIZE = SDL_AUDIO_BITSIZE(FORMAT);
constexpr int CHANNELS = 2;
constexpr int SAMPLES = 1024;
//constexpr int BYTES_PER_SAMPLE = (SAMPLE_SIZE * CHANNELS) / 8;
constexpr int BYTES_PER_SAMPLE = (SAMPLE_SIZE * CHANNELS) >> 3;
constexpr int BUF_LEN = SAMPLES * BYTES_PER_SAMPLE;


PlayThread::PlayThread(QObject *parent) : QObject(parent)
{
    m_bRunning = false;
}

PlayThread::~PlayThread()
{
}

void PlayThread::PrintVersion()
{
    SDL_version version;
    SDL_VERSION(&version);
    qDebug() << version.major << "." << version.minor << "." << version.patch;
}

static void AudioDataCallback(void* userdata, Uint8* stream, int len)
{
    //len的大小为  spec.samples * spec.format * spec.channels / 8
    PlayThread* p = static_cast<PlayThread*>(userdata);
    p->OnReadData(stream, len);
}

void PlayThread::OnReadData(uint8_t* stream, int len)
{
    /* 这里的清空重置不可缺少，否则音频播放出现刺耳的声音 */
    SDL_memset(stream, 0, len);
    //    qDebug() << "len=" << len;

    if (m_audioBuf.m_bufLen <= 0)
    {
        return;
    }

    m_audioBuf.m_useLen = (len > m_audioBuf.m_bufLen) ? m_audioBuf.m_bufLen : len;
    //填充数据
    SDL_MixAudio(stream, m_audioBuf.m_buf, m_audioBuf.m_useLen, SDL_MIX_MAXVOLUME);
    m_audioBuf.m_buf += m_audioBuf.m_useLen;
    m_audioBuf.m_bufLen -= m_audioBuf.m_useLen;
}

void PlayThread::Run()
{
    if (SDL_Init(SDL_INIT_AUDIO))
    {
        qDebug() << "failed to init sdl:" << SDL_GetError();
        emit ExistRun();
        return;
    }

    //音频参数
    SDL_AudioSpec spec;
    spec.freq = SAMPLE_RATE;//采样率
    spec.channels = CHANNELS;//声道数
    spec.format = FORMAT;//采样格式 s16le
    spec.samples = SAMPLES; //音频缓冲区的样本数量(必须为2的幂)
    spec.userdata = this;
    spec.callback = AudioDataCallback;//回调函数指针

    //打开设备
    if (SDL_OpenAudio(&spec, nullptr))
    {
        qDebug() << "failed to open audio:" << SDL_GetError();
        SDL_Quit();
        emit ExistRun();
        return;
    }

    //开始播放，0是取消暂停
    SDL_PauseAudio(0);

    //打开文件
    QFile file(FILE_NAME);
    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "failed to open file";
        SDL_CloseAudio();
        SDL_Quit();
        emit ExistRun();
        return;
    }

    uint8_t data[BUF_LEN];

    m_bRunning = true;
    while (m_bRunning)
    {
        if (m_audioBuf.m_bufLen > 0)
        {
            continue;
        }
        m_audioBuf.m_bufLen = file.read((char*)data, BUF_LEN);

        //所有的数据都已经读取完成了
        if (m_audioBuf.m_bufLen <= 0)
        {
            //剩余的样本数量
            int samples = m_audioBuf.m_useLen / BYTES_PER_SAMPLE;
            int ms = samples * 1000 / SAMPLE_RATE;
            SDL_Delay(ms);
            qDebug() << "delay ms=" << ms;
            break;
        }

        m_audioBuf.m_buf = data;
    }

    file.close();

    SDL_CloseAudio();

    //清除所有系统
    SDL_Quit();
    emit ExistRun();
}

void PlayThread::SetRunning(bool bRun)
{
    m_bRunning = bRun;
}
