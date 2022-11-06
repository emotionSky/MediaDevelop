QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    ffwindow.cpp

HEADERS += \
    ffwindow.h

FORMS += \
    ffwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:{
# 设置通用路径
DEPEND_HOME=../../3dpart

# 通过message打印信息
# message($${DEPEND_HOME})

# 使用系统的环境变量，使用 $$()；使用pro中定义的变量，使用 $${}
# message($$(PATH))

# 设置FFmpeg头文件位置
INCLUDEPATH += $${DEPEND_HOME}/include/ffmpeg

# 设置FFmpeg库文件位置
LIBS += -L$${DEPEND_HOME}/lib \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lpostproc \
        -lswresample \
        -lswscale
}

linux:{
# todo for linux
}

macx:{
# todo for mac
}

DISTFILES += \
    Ffmpeg_tutorial.txt
