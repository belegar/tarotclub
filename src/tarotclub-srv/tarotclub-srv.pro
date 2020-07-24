# TarotClub dedicated server project file
#
# TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
# Copyright (C) 2003-2999 - Anthony Rabine
# anthony@tarotclub.fr
#
# TarotClub is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# TarotClub is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
#

# ------------------------------------------------------------------------------
# Directories for generated files and base directory
# ------------------------------------------------------------------------------
BASE_DIR = $${PWD}/../..

CONFIG(debug, debug|release) {
debug:      DESTDIR = $$BASE_DIR/build/tarotclub-srv/debug
}

CONFIG(release, debug|release) {
release:    DESTDIR = $$BASE_DIR/build/tarotclub-srv/release
}

UI_DIR          = $$DESTDIR/ui
UI_HEADERS_DIR  = $$DESTDIR/include
UI_SOURCES_DIR  = $$DESTDIR/src
OBJECTS_DIR     = $$DESTDIR/obj
RCC_DIR         = $$DESTDIR/rcc
MOC_DIR         = $$DESTDIR/moc

# ------------------------------------------------------------------------------
# Compiler definitions
# ------------------------------------------------------------------------------
CONFIG += console warn_on
QMAKE_CXXFLAGS += -std=c++11

# Mainly for Duktape, the only source code in C
QMAKE_CFLAGS_DEBUG      += -O0 -pedantic -std=c99 -fstrict-aliasing -ggdb
QMAKE_CFLAGS_RELEASE    += -Os -pedantic -std=c99 -fstrict-aliasing -fomit-frame-pointer

# ------------------------------------------------------------------------------
# Targer definitions
# ------------------------------------------------------------------------------
TARGET = tarotclub-srv # name of the output executable

# Specific OS stuff
win32 {
    RC_FILE = icon.rc
    LIBS +=  libws2_32 -lpsapi
    DEFINES += USE_WINDOWS_OS
    # Let's make everything's static so that we don't need any DLL
    QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static -lpthread
}

unix {
    DEFINES += USE_UNIX_OS
    LIBS += -ldl
}

debug {
    DEFINES += TAROT_DEBUG
 #   DEFINES += DUK_OPT_DEBUG
   #  DEFINES += DUK_OPT_DDPRINT
}


# ------------------------------------------------------------------------------
# ICL files
# ------------------------------------------------------------------------------
ICL_DIR = $$BASE_DIR/lib/icl
include($$ICL_DIR/icl.pri)

# ------------------------------------------------------------------------------
# TarotClub core files
# ------------------------------------------------------------------------------
CONFIG += icl_zip icl_database icl_http
INCLUDEPATH += $$BASE_DIR/src
VPATH += $$BASE_DIR/src
include($$BASE_DIR/src/tarot_core.pri)

# ------------------------------------------------------------------------------
# Server files
# ------------------------------------------------------------------------------
INCLUDEPATH += $$BASE_DIR/src/tarotclub-srv
VPATH += $$BASE_DIR/src/tarotclub-srv

SOURCES +=  main.cpp Terminal.cpp tls_client.cpp \
    ServiceAiContest.cpp \
    ServiceStats.cpp \
    ServiceWebsiteConnection.cpp

HEADERS += Version.h Terminal.h IService.h tls_client.h \
    ServiceAiContest.h \
    ServiceStats.h \
    ServiceWebsiteConnection.h

# ------------------------------------------------------------------------------
# mBed TLS
# ------------------------------------------------------------------------------
VPATH += $$BASE_DIR/lib/mbedtls/include $$BASE_DIR/lib/mbedtls/library
INCLUDEPATH += $$BASE_DIR/lib/mbedtls/include

#DEFINES += -DMBEDTLS_CONFIG_FILE='<foo.h>'"

HEADERS +=  mbedtls/aes.h

SOURCES +=  aes.c \
aesni.c \
arc4.c \
aria.c \
asn1parse.c \
asn1write.c \
base64.c \
bignum.c \
blowfish.c \
camellia.c \
ccm.c \
certs.c \
chacha20.c \
chachapoly.c \
cipher.c \
cipher_wrap.c \
cmac.c \
ctr_drbg.c \
debug.c \
des.c \
dhm.c \
ecdh.c \
ecdsa.c \
ecjpake.c \
ecp.c \
ecp_curves.c \
entropy.c \
entropy_poll.c \
error.c \
gcm.c \
havege.c \
hkdf.c \
hmac_drbg.c \
md2.c \
md4.c \
md5.c \
md.c \
md_wrap.c \
memory_buffer_alloc.c \
net_sockets.c \
nist_kw.c \
oid.c \
padlock.c \
pem.c \
pk.c \
pkcs11.c \
pkcs12.c \
pkcs5.c \
pkparse.c \
pk_wrap.c \
pkwrite.c \
platform.c \
platform_util.c \
poly1305.c \
ripemd160.c \
rsa.c \
rsa_internal.c \
sha1.c \
sha256.c \
sha512.c \
ssl_cache.c \
ssl_ciphersuites.c \
ssl_cli.c \
ssl_cookie.c \
ssl_srv.c \
ssl_ticket.c \
ssl_tls.c \
threading.c \
timing.c \
version.c \
version_features.c \
x509.c \
x509_create.c \
x509_crl.c \
x509_crt.c \
x509_csr.c \
x509write_crt.c \
x509write_csr.c \
xtea.c \

# End of project file
