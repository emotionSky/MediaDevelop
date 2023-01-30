#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QObject>

class PlayThread  : public QObject
{
    Q_OBJECT
public:
    explicit PlayThread(QObject *parent = nullptr);
    virtual ~PlayThread();
    void PrintVersion();
    void OnReadData(uint8_t* stream, int len);
    void Run();
    void SetRunning(bool bRun);

signals:
    void ExistRun();

private:
    struct AudioBuf
    {
        int m_bufLen;
        int m_useLen;
        uint8_t* m_buf;
    };

private:
    bool m_bRunning;
    AudioBuf m_audioBuf;
};

#endif // PLAYTHREAD_H
