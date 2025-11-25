QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += $$PWD

LIBS += -L$$PWD
LIBS += -lws2_32 -ldetours

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    absmanager.cpp \
    cmdmanager.cpp \
    kernelmanager.cpp \
    loginhelper.cpp \
    main.cpp \
    mainwindow.cpp \
    processmanager.cpp \
    registrymanager.cpp \
    tcpclient.cpp \
    windowmanager.cpp

HEADERS += \
    absmanager.h \
    cmdmanager.h \
    common.h \
    kernelmanager.h \
    loginhelper.h \
    mainwindow.h \
    processhelper.h \
    processmanager.h \
    registrymanager.h \
    tcpclient.h \
    windowmanager.h \
    detours.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

