#[[
 CMake file to build ICL and TarotClub source files on Android
]]

cmake_minimum_required(VERSION 3.4.1)

# New path definition
set(ICL_DIR ~/git/tarotclub/lib/icl)

# Switch on the C++0x14 standard for the compiler
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_library(tarotclub_server SHARED
            tarotclub_main.c)

# Add the ICL library and specify the source files to build
add_library(icl SHARED ${ICL_DIR}/util/Util.cpp)


# Include libraries needed for TarotClub lib
target_link_libraries(tarotclub_server
                      android
                      icl
                      log)

# End of Cmake file
