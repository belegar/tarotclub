# ------------------------------------------------------------------------------
# Util files
# ------------------------------------------------------------------------------
HEADERS += Log.h \
    Observer.h \
    Util.h \
    ThreadQueue.h \
    GetOptions.h \
    Value.h \
    UniqueId.h \
    Semaphore.h \
    Console.h \
    IEventLoop.h \
    EventLoop.h

SOURCES += Log.cpp \
    Util.cpp \
    Value.cpp \
    UniqueId.cpp \
    Console.cpp \
    EventLoop.cpp

# ------------------------------------------------------------------------------
# I/O files
# ------------------------------------------------------------------------------
HEADERS += ByteStreamReader.h \
    ByteStreamWriter.h \
    ByteArray.h

SOURCES += ByteArray.cpp \
    ByteStreamReader.cpp \
    ByteStreamWriter.cpp

# ------------------------------------------------------------------------------
# Network files
# ------------------------------------------------------------------------------
HEADERS += TcpSocket.h \
    TcpServer.h \
    TcpClient.h \
    WebSocket.h \
    TcpServerBase.h

SOURCES += TcpSocket.cpp \
    TcpServer.cpp \
    TcpClient.cpp \
    WebSocket.cpp \
    TcpServerBase.cpp

# ------------------------------------------------------------------------------
# Protocol files
# ------------------------------------------------------------------------------
HEADERS += Http.h

SOURCES += Http.cpp

# ------------------------------------------------------------------------------
# Database files
# ------------------------------------------------------------------------------
HEADERS += CouchDb.h \
            DataBase.h

SOURCES += CouchDb.cpp \
            DataBase.cpp \
            sqlite3.c

# ------------------------------------------------------------------------------
# Security files
# ------------------------------------------------------------------------------
HEADERS += Base64.h \
    Sha1.h

SOURCES += Base64.cpp \
    Sha1.cpp

# ------------------------------------------------------------------------------
# ZIP files
# ------------------------------------------------------------------------------
HEADERS += Zip.h

SOURCES += Zip.cpp

# ------------------------------------------------------------------------------
# JSON files
# ------------------------------------------------------------------------------
HEADERS += JsonWriter.h \
   JsonReader.h \
   JsonValue.h

SOURCES += JsonWriter.cpp \
    JsonReader.cpp \
    JsonValue.cpp


# ------------------------------------------------------------------------------
# JSEngine and JSON files
# ------------------------------------------------------------------------------
HEADERS += duktape.h duk_config.h \
    JSEngine.h \
    IScriptEngine.h

SOURCES += duktape.c \
    JSEngine.cpp

CONFIG(debug, debug|release) {
    win32 {
        SOURCES += duk_trans_socket_windows.c
    }
}

VPATH += $$ICL_DIR/network
VPATH += $$ICL_DIR/util
VPATH += $$ICL_DIR/zip
VPATH += $$ICL_DIR/security
VPATH += $$ICL_DIR/jsengine
VPATH += $$ICL_DIR/protocol
VPATH += $$ICL_DIR/json
VPATH += $$ICL_DIR/io
VPATH += $$ICL_DIR/db

INCLUDEPATH += $$ICL_DIR/network
INCLUDEPATH += $$ICL_DIR/util
INCLUDEPATH += $$ICL_DIR/zip
INCLUDEPATH += $$ICL_DIR/security
INCLUDEPATH += $$ICL_DIR/jsengine
INCLUDEPATH += $$ICL_DIR/protocol
INCLUDEPATH += $$ICL_DIR/json
INCLUDEPATH += $$ICL_DIR/io
INCLUDEPATH += $$ICL_DIR/db


