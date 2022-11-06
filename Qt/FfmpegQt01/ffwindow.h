#ifndef FFWINDOW_H
#define FFWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class FfWindow;
}
QT_END_NAMESPACE

class FfWindow : public QMainWindow
{
    Q_OBJECT

public:
    FfWindow(QWidget *parent = nullptr);
    ~FfWindow();

private slots:
    void on_audioButton_clicked();

private:
    Ui::FfWindow *ui;
};
#endif // FFWINDOW_H
