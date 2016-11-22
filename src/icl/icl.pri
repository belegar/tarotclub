# ------------------------------------------------------------------------------
# Util files
# ------------------------------------------------------------------------------
HEADERS += $$ICL_DIR/util/Log.h \
    $$ICL_DIR/util/Observer.h \
    $$ICL_DIR/util/Util.h \
    $$ICL_DIR/util/ThreadQueue.h \
    $$ICL_DIR/util/GetOptions.h \
    $$ICL_DIR/util/Value.h \
    $$ICL_DIR/util/UniqueId.h

SOURCES += $$ICL_DIR/util/Log.cpp \
    $$ICL_DIR/util/Util.cpp \
    $$ICL_DIR/util/Value.cpp \
    $$ICL_DIR/util/UniqueId.cpp

# ------------------------------------------------------------------------------
# I/O files
# ------------------------------------------------------------------------------
HEADERS += $$ICL_DIR/io/ByteStreamReader.h \
    $$ICL_DIR/io/ByteStreamWriter.h \
    $$ICL_DIR/io/ByteArray.h

SOURCES += $$ICL_DIR/io/ByteArray.cpp \
    $$ICL_DIR/io/ByteStreamReader.cpp \
    $$ICL_DIR/io/ByteStreamWriter.cpp

# ------------------------------------------------------------------------------
# Network files
# ------------------------------------------------------------------------------
HEADERS += $$ICL_DIR/network/TcpSocket.h \
    $$ICL_DIR/network/TcpServer.h \
    $$ICL_DIR/network/ TcpClient.h \
    $$ICL_DIR/network/WebSocket.h \
    $$ICL_DIR/network/TcpServerBase.h

SOURCES += $$ICL_DIR/network/TcpSocket.cpp \
    $$ICL_DIR/network/TcpServer.cpp \
    $$ICL_DIR/network/TcpClient.cpp \
    $$ICL_DIR/network/WebSocket.cpp \
    $$ICL_DIR/network/TcpServerBase.cpp

# ------------------------------------------------------------------------------
# Protocol files
# ------------------------------------------------------------------------------

HEADERS += $$ICL_DIR/protocol/Http.h

SOURCES += $$ICL_DIR/protocol/Http.cpp

# ------------------------------------------------------------------------------
# Database files
# ------------------------------------------------------------------------------

HEADERS += $$ICL_DIR/db/CouchDb.h \
            $$ICL_DIR/db/DataBase.h

SOURCES += $$ICL_DIR/db/CouchDb.cpp \
            $$ICL_DIR/db/DataBase.cpp \
            $$ICL_DIR/db/sqlite3.c


# ------------------------------------------------------------------------------
# Security files
# ------------------------------------------------------------------------------

HEADERS += $$ICL_DIR/security/Base64.h \
    $$ICL_DIR/security/Sha1.h

SOURCES += $$ICL_DIR/security/Base64.cpp \
    $$ICL_DIR/security/Sha1.cpp


# ------------------------------------------------------------------------------
# ZIP files
# ------------------------------------------------------------------------------

HEADERS += $$ICL_DIR/zip/Zip.h

SOURCES += $$ICL_DIR/zip/Zip.cpp

# ------------------------------------------------------------------------------
# JSON files
# ------------------------------------------------------------------------------
HEADERS += $$ICL_DIR/json/JsonWriter.h \
    $$ICL_DIR/json/JsonReader.h \
    $$ICL_DIR/json/JsonValue.h

SOURCES += $$ICL_DIR/json/JsonWriter.cpp \
    $$ICL_DIR/json/JsonReader.cpp \
    $$ICL_DIR/json/JsonValue.cpp


# ------------------------------------------------------------------------------
# JSEngine and JSON files
# ------------------------------------------------------------------------------

HEADERS += $$ICL_DIR/jsengine/duktape.h \
    $$ICL_DIR/jsengine/JSEngine.h \
    $$ICL_DIR/jsengine/IScriptEngine.h

SOURCES += $$ICL_DIR/jsengine/duktape.c \
    $$ICL_DIR/jsengine/JSEngine.cpp

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


