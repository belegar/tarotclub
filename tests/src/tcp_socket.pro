
VPATH += $${PWD}/../../src/library
VPATH += $${PWD}/../../src
INCLUDEPATH += $${PWD}/../../src/library
INCLUDEPATH += $${PWD}/../../src

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

win32 {
    LIBS +=  libws2_32
    DEFINES += USE_WINDOWS_OS
}
unix {
    LIBS += -lpthread
    DEFINES += USE_UNIX_OS
}

SOURCES += tst_tcp_socket.cpp \
    TcpSocket.cpp \
    TcpServer.cpp \
    TcpClient.cpp

HEADERS += \
    TcpSocket.h \
    TcpServer.h \
    TcpClient.h \
    Observer.h


