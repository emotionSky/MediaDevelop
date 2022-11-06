#ifndef RECORDAUDIOTHREAD_H
#define RECORDAUDIOTHREAD_H

#include <QObject>

class RecordAudioThread : public QObject
{
    Q_OBJECT
public:
    explicit RecordAudioThread(QObject *parent = nullptr);
    virtual ~RecordAudioThread();

signals:
    void error_signal();

public:
    bool IsRun()
    {
        return m_bRunning;
    }
    void Start();
    void Stop();
    void Run();

private:
    bool m_bRunning;
};

#endif // RECORDAUDIOTHREAD_H
