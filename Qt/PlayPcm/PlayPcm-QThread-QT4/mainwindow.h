#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "playthread.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void StopThread();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    PlayThread m_player;
    uint8_t* m_pData;
    int m_len;
};
#endif // MAINWINDOW_H
