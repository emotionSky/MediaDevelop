#include "ffwindow.h"

#include <QApplication>
extern "C" {
    // 设备
#include <libavdevice/avdevice.h>
}

int main(int argc, char *argv[])
{
    // 注册设备，全局只需要执行一次
    avdevice_register_all();
    QApplication a(argc, argv);
    FfWindow w;
    w.show();
    return a.exec();
}
