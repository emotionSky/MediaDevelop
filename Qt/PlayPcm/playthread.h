#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <thread>
#include <QObject>

class PlayThread  : public QObject
{
    Q_OBJECT
public:
    explicit PlayThread(QObject *parent = nullptr);
    virtual ~PlayThread();

    bool GetStatus() const;
    void Start();
    void Stop();
    void PrintVersion();
    void OnReadData(uint8_t* stream, int len);
    void Run();

signals:
    void ExistThreadSignal();

private:
    struct AudioBuf
    {
        int m_bufLen;
        int m_useLen;
        uint8_t* m_buf;
    };

private:
    bool m_bRunning;
    std::thread* m_pThread;
    AudioBuf m_audioBuf;
};

#endif // PLAYTHREAD_H
