#-------------------------------------------------
#
# Project created by QtCreator 2014-03-25T12:07:35
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = express_server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    cli.cpp \
    expressmain.cpp \
    express_udp.cpp \
    express.cpp \
    si570.cpp \
    null.cpp \
    buffers.cpp

HEADERS += \
    clitypes.h \
    cli.h \
    express_server.h \
    express.h \
    si570.h

unix{
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0
}
