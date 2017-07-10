#[[
 CMake file to build ICL and TarotClub source files on Android
]]

cmake_minimum_required(VERSION 3.4.1)

# New path definition
set(ICL_DIR ~/git/tarotclub/lib/icl)
set(TAROTCLUB_DIR ~/git/tarotclub/src)

set(ICL_DIR_UTILS   ${ICL_DIR}/util)
set(ICL_DIR_NET     ${ICL_DIR}/network)
set(ICL_DIR_ZIP     ${ICL_DIR}/zip)
set(ICL_DIR_SEC     ${ICL_DIR}/security)
set(ICL_DIR_JS      ${ICL_DIR}/jsengine)
set(ICL_DIR_PROTO   ${ICL_DIR}/protocol)
set(ICL_DIR_JSON    ${ICL_DIR}/json)
set(ICL_DIR_IO      ${ICL_DIR}/io)
set(ICL_DIR_DB      ${ICL_DIR}/db)

# Switch on the C++0x14 standard for the compiler
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_library(tarotclub_server SHARED
            tarotclub_main.c
            filesystem.c
            file_server.cpp)

include_directories(${ICL_DIR_UTILS} ${ICL_DIR_NET} ${ICL_DIR_ZIP} ${ICL_DIR_SEC} ${ICL_DIR_JS} ${ICL_DIR_PROTO} ${ICL_DIR_JSON} ${ICL_DIR_IO} ${ICL_DIR_DB} ${TAROTCLUB_DIR})

set(ICL_DIR_UTILS_SRC ${ICL_DIR_UTILS}/Util.cpp
                      ${ICL_DIR_UTILS}/Log.cpp
                      ${ICL_DIR_UTILS}/Value.cpp
                      ${ICL_DIR_UTILS}/UniqueId.cpp
                      ${ICL_DIR_UTILS}/Console.cpp
                      ${ICL_DIR_UTILS}/EventLoop.cpp)

set(ICL_DIR_NET_SRC ${ICL_DIR_NET}/TcpSocket.cpp
                    ${ICL_DIR_NET}/TcpServer.cpp
                    ${ICL_DIR_NET}/TcpClient.cpp
                    ${ICL_DIR_NET}/WebSocket.cpp
                    ${ICL_DIR_NET}/TcpServerBase.cpp)

set(ICL_DIR_ZIP_SRC ${ICL_DIR_ZIP}/Zip.cpp)


set(ICL_DIR_SEC_SRC ${ICL_DIR_SEC}/Base64.cpp
                    ${ICL_DIR_SEC}/Sha1.cpp)

set(ICL_DIR_JS_SRC  ${ICL_DIR_JS}/JSEngine.cpp
                    ${ICL_DIR_JS}/duktape.c)

set(ICL_DIR_PROTO_SRC ${ICL_DIR_PROTO}/Http.cpp)

set(ICL_DIR_JSON_SRC    ${ICL_DIR_JSON}/JsonWriter.cpp
                        ${ICL_DIR_JSON}/JsonReader.cpp
                        ${ICL_DIR_JSON}/JsonValue.cpp)

set(ICL_DIR_IO_SRC  ${ICL_DIR_IO}/ByteArray.cpp
                    ${ICL_DIR_IO}/ByteStreamReader.cpp
                    ${ICL_DIR_IO}/ByteStreamWriter.cpp)

set(ICL_DIR_DB_SRC  ${ICL_DIR_DB}/CouchDb.cpp
                    ${ICL_DIR_DB}/DataBase.cpp
                    ${ICL_DIR_DB}/sqlite3.c)

# Add the ICL library and specify the source files to build
add_library(icl SHARED  ${ICL_DIR_UTILS_SRC}
                        ${ICL_DIR_NET_SRC}
                        ${ICL_DIR_SEC_SRC}
                        ${ICL_DIR_ZIP_SRC}
                        ${ICL_DIR_JS_SRC}
                        ${ICL_DIR_PROTO_SRC}
                        ${ICL_DIR_JSON_SRC}
                        ${ICL_DIR_IO_SRC}
                        ${ICL_DIR_DB_SRC})

# Add the TarotCore library and specify the source files to build
add_library(tarotcore SHARED    ${TAROTCLUB_DIR}/ServerConfig.cpp
                                ${TAROTCLUB_DIR}/ClientConfig.cpp
                                ${TAROTCLUB_DIR}/TournamentConfig.cpp
                                ${TAROTCLUB_DIR}/DealFile.cpp
                                ${TAROTCLUB_DIR}/Deck.cpp
                                ${TAROTCLUB_DIR}/Card.cpp
                                ${TAROTCLUB_DIR}/Player.cpp
                                ${TAROTCLUB_DIR}/Bot.cpp
                                ${TAROTCLUB_DIR}/Common.cpp
                                ${TAROTCLUB_DIR}/Engine.cpp
                                ${TAROTCLUB_DIR}/Deal.cpp
                                ${TAROTCLUB_DIR}/Protocol.cpp
                                ${TAROTCLUB_DIR}/PlayingTable.cpp
                                ${TAROTCLUB_DIR}/Score.cpp
                                ${TAROTCLUB_DIR}/System.cpp
                                ${TAROTCLUB_DIR}/Users.cpp
                                ${TAROTCLUB_DIR}/Lobby.cpp
                                ${TAROTCLUB_DIR}/BotManager.cpp
                                ${TAROTCLUB_DIR}/Server.cpp
                                ${TAROTCLUB_DIR}/BasicClient.cpp
                                ${TAROTCLUB_DIR}/Network.cpp
                                ${TAROTCLUB_DIR}/Session.cpp
                                ${TAROTCLUB_DIR}/Identity.cpp)


target_link_libraries(tarotcore icl)

# Include libraries needed for TarotClub lib
target_link_libraries(tarotclub_server
                      android
                      icl
                      tarotcore
                      log)

# End of Cmake file
