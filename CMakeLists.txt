project (bulletmind)

cmake_minimum_required(VERSION 3.18)

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
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BITS 64)
    message(STATUS "Bits: 64-bit")
else()
    set(BITS 32)
    message(STATUS "Bits: 32-bit")
endif()

message(STATUS "cmake install destinations (when running cmake --install):\n\
    CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}\n\
    CMAKE_INSTALL_INCLUDEDIR = ${CMAKE_INSTALL_INCLUDEDIR}\n\
    CMAKE_INSTALL_LIBDIR = ${CMAKE_INSTALL_LIBDIR}\n\
    CMAKE_INSTALL_BINDIR = ${CMAKE_INSTALL_BINDIR}")

option(BM_BUILD_32BIT "build bulletmind as 32-bit" OFF)
option(BM_USE_CIMGUI "use cimgui wrapper for DearIMGUI" ON)
option(BM_USE_SDL2 "Use SDL2?" ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-DBM_DEBUG)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Od /Zi")
    endif()
    if (BM_BUILD_32BIT)
        set(TARGET_LINK_OPTS ${TARGET_LINK_OPTS} "/MACHINE:X86")
    else()
        set(TARGET_LINK_OPTS ${TARGET_LINK_OPTS} "/MACHINE:X64")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    message("Compiler: ${CMAKE_CXX_COMPILER_ID}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-int-to-pointer-cast")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-DBM_DEBUG)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    endif()
    if (BM_BUILD_32BIT)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
    endif()
endif()

set(TARGET_INCLUDE_DIRS
    src
    thirdparty/
    thirdparty/tomlc99)
if (BM_USE_SDL2)
    list(APPEND TARGET_INCLUDE_DIRS thirdparty/SDL2/include)
    list(APPEND TARGET_COMPILE_DEFS -DBM_USE_SDL2)
endif()
if (BM_USE_CIMGUI)
    list(APPEND TARGET_INCLUDE_DIRS thirdparty/cimgui thirdparty/cimgui/generator/output)
    list(APPEND TARGET_COMPILE_DEFS -DCM_USE_CIMGUI)
endif()

# murmur2
set(MURMURHASH2_SOURCES
    thirdparty/murmur2/MurmurHash2.h
    thirdparty/murmur2/MurmurHash2.c
)
list(APPEND TARGET_INCLUDE_DIRS thirdparty/murmur2)

# tomlc99
set(TOMLC99_SOURCES
    thirdparty/tomlc99/toml.h
    thirdparty/tomlc99/toml.c
)

# core
set(BM_CORE_HEADERS
    src/core/algorithm.h
    src/core/binary.h
    src/core/buffer.h
    src/core/export.h
    src/core/file_io.h
    src/core/intrinsics.h
    src/core/logger.h
    src/core/memory.h
    src/core/path.h
    src/core/random.h
    src/core/rect.h
    src/core/scancode.h
    src/core/string.h
    src/core/time_convert.h
    src/core/timer.h
    src/core/toml_config.h
    src/core/types.h
    src/core/utils.h
    src/core/vector.h
)
set(BM_CORE_SOURCES
    src/core/binary.c
    src/core/buffer.c
    src/core/file_io.c
    src/core/hashmap.c
    src/core/logger.c
    src/core/memory.c
    src/core/path.c
    src/core/string.c
    src/core/timer.c
    src/core/toml_config.c
    src/core/utils.c
)

# math
set(BM_MATH_HEADERS
    src/math/types.h
    src/math/utils.h
    src/math/bounds.h
    src/math/vec2.h
    src/math/vec3.h
    src/math/vec4.h
    src/math/mat4.h
    src/math/quat.h
)

# platform
set(BM_PLATFORM_HEADERS
    src/platform/console.h
    src/platform/gamepad.h
    src/platform/input.h
    src/platform/keyboard.h
    src/platform/keyboard-scancode.h
    src/platform/mouse.h
    src/platform/platform.h
    src/platform/utf8.h
)
set(BM_PLATFORM_SOURCES
    src/platform/input.c
    src/platform/platform.c
    src/platform/utf8.c
)

# gfx
set(BM_GFX_HEADERS
    src/gfx/enums.h
    src/gfx/gfx.h
    src/gfx/camera.h
)
set(BM_GFX_SOURCES
    src/gfx/gfx.c
    src/gfx/camera.c
    src/gfx/sprite.c
)

set(BM_MEDIA_HEADERS
    src/media/enums.h
    src/media/image.h
    src/media/targa.h
)
set(BM_MEDIA_SOURCES
    src/media/image.c
)

# gui
set(BM_GUI_HEADERS
    src/gui/gui.h
)
set(BM_GUI_SOURCES
    src/gui/gui.c
)
if (BM_USE_SDL2)
    list(APPEND BM_GUI_SOURCES src/gui/gui_sdl2.c)
endif()

# game
set(BM_GAME_HEADERS
    # src/audio.h
    src/game/app.h
    src/game/asset.h
    src/game/command.h
    src/game/entity.h
    src/game/scene.h
    # src/engine.h
    # src/entity.h
    # src/font.h
    # src/render.h
    # src/resource.h
    # src/sprite.h
)
set(BM_GAME_SOURCES
    # src/audio.c
    src/game/app.c
    src/game/asset.c
    src/game/command.c
    src/game/entity.c
    # src/engine.c
    # src/entity.c
    # src/font.c
    src/game/main.c
    src/game/scene.c
    # src/render.c
    # src/resource.c
    # src/sprite.c
)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    list(APPEND TARGET_COMPILE_DEFS
        -DBM_WINDOWS
        -DUNICODE
        -D_UNICODE
        -D_CRT_SECURE_NO_WARNINGS)
    if (BM_USE_CIMGUI)
        list(APPEND TARGET_COMPILE_DEFS
            -DBM_USE_CIMGUI
            -DCIMGUI_DEFINE_ENUMS_AND_STRUCTS
            -DCIMGUI_USE_WIN32
            -DCIMGUI_USE_DX11
            -DCIMGUI_USE_SDL2)
    endif()
        # -DSDL_AUDIODRIVER=dsound)
    list(APPEND BM_PLATFORM_HEADERS
        src/platform/keyboard-vkey-win32.h)
    list(APPEND BM_PLATFORM_SOURCES
        src/platform/platform-win32.c)

    list(APPEND BM_GFX_HEADERS
        src/gfx/dxguids.h
        src/gfx/win32_types.h
        src/gfx/gfx_d3d11.h
        src/gfx/gfx_d3d11_input_layout.h)
    list(APPEND BM_GFX_SOURCES
        src/gfx/gfx_d3d11.c)
    list(APPEND BM_GUI_SOURCES
        src/gui/gui_win32.c)

    if (BM_BUILD_32BIT)
        if (BM_USE_SDL2)
            list(APPEND TARGET_LINK_DIRS
                thirdparty/SDL2/lib/x86)
        endif()
        set(BM_ARCH "x86")
    else()
        if (BM_USE_SDL2)
            list(APPEND TARGET_LINK_DIRS
                thirdparty/SDL2/lib/x64)
        endif()
        if (BM_USE_CIMGUI)
            list(APPEND TARGET_LINK_DIRS thirdparty/cimgui/build)
        endif()
        set(BM_ARCH "x64")
    endif()

    list(APPEND TARGET_LINK_LIBS
        kernel32
        DbgHelp)
    if (BM_USE_CIMGUI)
        list(APPEND TARGET_LINK_LIBS cimgui.lib)
    endif()
    if (BM_USE_SDL2)
        list(APPEND TARGET_LINK_LIBS
            SDL2main.lib
            SDL2.lib
            SDL2_image.lib
            SDL2_mixer.lib)
    endif()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND TARGET_COMPILE_DEFS
        -DBM_DARWIN)
    list(APPEND BM_PLATFORM_SOURCES
        src/platform/platform-darwin.c
        src/platform/platform-posix.c)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND TARGET_COMPILE_DEFS
        -DBM_LINUX)
endif()

set(BM_TARGET_SOURCES
    ${MURMURHASH2_SOURCES}
    ${TOMLC99_SOURCES}
    ${BM_CORE_HEADERS}
    ${BM_CORE_SOURCES}
    ${BM_MATH_HEADERS}
    ${BM_PLATFORM_HEADERS}
    ${BM_PLATFORM_SOURCES}
    ${BM_GFX_HEADERS}
    ${BM_GFX_SOURCES}
    ${BM_GUI_HEADERS}
    ${BM_GUI_SOURCES}
    ${BM_MEDIA_HEADERS}
    ${BM_MEDIA_SOURCES}
    ${BM_GAME_HEADERS}
    ${BM_GAME_SOURCES})

# bulletmind
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(bulletmind WIN32 ${BM_TARGET_SOURCES})
    # set_target_properties(bulletmind PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
    set(TARGET_LINK_OPTS ${TARGET_LINK_OPTS} "/SUBSYSTEM:CONSOLE")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if (BM_USE_SDL2)
        find_library(SDL2_LIBRARY NAME SDL2 REQUIRED)
        find_library(SDL2_MIXER_LIBRARY NAME SDL2_mixer REQUIRED)
        find_library(SDL2_IMAGE_LIBRARY NAME SDL2_image REQUIRED)
        set(SDL2_INCLUDE_DIR /Library/Frameworks/SDL2.framework/Headers)
        set(SDL2_MIXER_INCLUDE_DIR /Library/Frameworks/SDL2_mixer.framework/Headers)
        set(SDL2_IMAGE_INCLUDE_DIR /Library/Frameworks/SDL2_image.framework/Headers)
        list(APPEND TARGET_INCLUDE_DIRS
            ${SDL2_INCLUDE_DIR}
            ${SDL2_MIXER_INCLUDE_DIR}
            ${SDL2_IMAGE_INCLUDE_DIR})
        list(APPEND TARGET_LINK_LIBS
            ${SDL2_LIBRARY}
            ${SDL2_MIXER_LIBRARY}
            ${SDL2_IMAGE_LIBRARY})
    endif()
    add_executable(bulletmind ${BM_TARGET_SOURCES})
else()
    add_executable(bulletmind ${BM_TARGET_SOURCES})
endif()

set_property(TARGET bulletmind PROPERTY C_STANDARD 11)
target_compile_definitions(bulletmind PRIVATE ${TARGET_COMPILE_DEFS})
target_include_directories(bulletmind PRIVATE ${TARGET_INCLUDE_DIRS})
target_link_directories(bulletmind PRIVATE ${TARGET_LINK_DIRS})
target_link_libraries(bulletmind PRIVATE ${TARGET_LINK_LIBS})
target_link_options(bulletmind PRIVATE ${TARGET_LINK_OPTS})

# post-build commands
if (BM_USE_SDL2 AND CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/libFLAC-8.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/libFLAC-8.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/libjpeg-9.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/libjpeg-9.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/libmodplug-1.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/libmodplug-1.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/libmpg123-0.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/libmpg123-0.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/libogg-0.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/libogg-0.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/libopus-0.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/libopus-0.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/libopusfile-0.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/libopusfile-0.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/libpng16-16.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/libpng16-16.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/libtiff-5.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/libtiff-5.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/libvorbis-0.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/libvorbis-0.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/libvorbisfile-3.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/libvorbisfile-3.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/libwebp-7.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/libwebp-7.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/SDL2.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/SDL2_image.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2_image.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/SDL2_mixer.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2_mixer.dll)
    add_custom_command(TARGET bulletmind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib/${BM_ARCH}/zlib1.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/zlib1.dll)
endif()
# copy assets
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)
# copy configs
# add_custom_command(TARGET bulletmind POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_CURRENT_SOURCE_DIR}/config
#     $<TARGET_FILE_DIR:${PROJECT_NAME}>/config)

add_executable(bm_tests
    src/core/tests.c
    ${MURMURHASH2_SOURCES}
    ${TOMLC99_SOURCES}
    ${BM_CORE_HEADERS}
    ${BM_CORE_SOURCES}
    ${BM_MATH_HEADERS}
    ${BM_PLATFORM_HEADERS}
    ${BM_PLATFORM_SOURCES}
    ${BM_GFX_HEADERS}
    ${BM_GFX_SOURCES}
    ${BM_GUI_HEADERS}
    ${BM_GUI_SOURCES}
    ${BM_MEDIA_HEADERS}
    ${BM_MEDIA_SOURCES}
)
set_property(TARGET bm_tests PROPERTY C_STANDARD 11)
target_compile_definitions(bm_tests PRIVATE -D_CRT_SECURE_NO_WARNINGS)
target_include_directories(bm_tests PRIVATE
    ${TARGET_INCLUDE_DIRS}
    thirdparty/psi)
target_link_directories(bm_tests PRIVATE ${TARGET_LINK_DIRS})
target_link_libraries(bm_tests PRIVATE ${TARGET_LINK_LIBS})
target_link_options(bm_tests PRIVATE ${TARGET_LINK_OPTS})
