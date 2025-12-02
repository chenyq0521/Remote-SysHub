QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -lws2_32

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cmddialog.cpp \
    filedialog.cpp \
    main.cpp \
    processdialog.cpp \
    registrydatadialog.cpp \
    registrydialog.cpp \
    serverdlg.cpp \
    tcpserver.cpp \
    tcpworker.cpp \
    windowdialog.cpp

HEADERS += \
    cmddialog.h \
    common.h \
    filedialog.h \
    processdialog.h \
    registrydatadialog.h \
    registrydialog.h \
    serverdlg.h \
    tcpserver.h \
    tcpworker.h \
    windowdialog.h

FORMS += \
    cmddialog.ui \
    filedialog.ui \
    processdialog.ui \
    registrydatadialog.ui \
    registrydialog.ui \
    serverdlg.ui \
    windowdialog.ui

TRANSLATIONS += \
    RServer_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

