#-------------------------------------------------
#
# Project created by QtCreator 2017-05-11T18:27:59
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = projectPDS
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    user.cpp \
    node.cpp \
    file.cpp \
    directory.cpp \
    settings.cpp \
    discovery.cpp \
    users.cpp \
    settingswindow.cpp \
    choosingdirectorywindow.cpp \
    userswindow.cpp \
    transfer.cpp \
    workerthread.cpp \
    socketthread.cpp \
    tcpserver.cpp \
    tcpclient.cpp

HEADERS  += \
    user.h \
    node.h \
    file.h \
    directory.h \
    settings.h \
    discovery.h \
    users.h \
    settingswindow.h \
    choosingdirectorywindow.h \
    userswindow.h \
    transfer.h \
    workerthread.h \
    socketthread.h \
    tcpserver.h \
    tcpclient.h

FORMS    += \
    settingswindow.ui \
    choosingdirectorywindow.ui \
    userswindow.ui \
    transfer.ui

RESOURCES += \
    res.qrc
