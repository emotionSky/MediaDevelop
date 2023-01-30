#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QObject>
#include <QThread>

class PlayThread  : public QThread
{
    Q_OBJECT
public:
    explicit PlayThread(QObject *parent = nullptr);
    virtual ~PlayThread();
    void PrintVersion();
    void OnReadData(uint8_t* stream, int len);
    virtual void run() override;

private:
    struct AudioBuf
    {
        int m_bufLen;
        int m_useLen;
        uint8_t* m_buf;
    };

private:
    AudioBuf m_audioBuf;
};

#endif // PLAYTHREAD_H
