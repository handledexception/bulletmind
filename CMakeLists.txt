project(Bulletmind)

cmake_minimum_required(VERSION 3.18)

set(ROOT_DIR        ${CMAKE_SOURCE_DIR})
set(GAME_DIR        ${CMAKE_SOURCE_DIR}/game)
set(LIBMIND_DIR     ${CMAKE_SOURCE_DIR}/libmind)
set(THIRDPARTY_DIR  ${CMAKE_SOURCE_DIR}/thirdparty)

message(STATUS "Bulletmind (c) 2023 Paul Hindt")
message(STATUS "https://github.com/handledexception/bulletmind")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Arch: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    message(STATUS "Windows API Version: ${CMAKE_SYSTEM_VERSION}")
    string(REPLACE "." ";" AJA_WIN_API_VER "${CMAKE_SYSTEM_VERSION}")
    list(GET AJA_WIN_API_VER 0 AJA_WIN_API_VER_MAJOR)
    list(GET AJA_WIN_API_VER 1 AJA_WIN_API_VER_MINOR)
    list(GET AJA_WIN_API_VER 2 AJA_WIN_API_VER_BUILD)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message (STATUS "macOS Architectures: ${CMAKE_OSX_ARCHITECTURES}")
    message (STATUS "macOS Deployment Target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(BM_LINUX_DISTRO "TODO")
    message(STATUS "Linux distro: ${BM_LINUX_DISTRO}")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

message(STATUS "cmake --install paths:\n\
    CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}\n\
    CMAKE_INSTALL_INCLUDEDIR = ${CMAKE_INSTALL_INCLUDEDIR}\n\
    CMAKE_INSTALL_LIBDIR = ${CMAKE_INSTALL_LIBDIR}\n\
    CMAKE_INSTALL_BINDIR = ${CMAKE_INSTALL_BINDIR}")

option(BM_BUILD_32BIT "build bulletmind as 32-bit" OFF)
option(BM_USE_CIMGUI "use cimgui wrapper for DearIMGUI" ON)
option(BM_USE_SDL2 "Use SDL2?" ON)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Bits: 64-bit")
    set(BM_BITS 64)
    set(BM_ARCH x64)
else()
    message(STATUS "Bits: 32-bit")
    set(BM_BITS 32)
    set(BM_ARCH x86)
    set(BM_BUILD_32BIT ON)
endif()

add_subdirectory(thirdparty)
add_subdirectory(libmind)
add_subdirectory(game)
