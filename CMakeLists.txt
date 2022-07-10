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
    deps/aja/ajalibraries
    deps/murmur2
    deps/tomlc99
    deps/SDL2/include)

# murmur2
set(MURMURHASH2_SOURCES
    deps/murmur2/MurmurHash2.h
    deps/murmur2/MurmurHash2.c)

# tomlc99
set(TOMLC99_SOURCES
    deps/tomlc99/toml.h
    deps/tomlc99/toml.c)

# core
set(BM_CORE_HEADERS
    src/core/algorithm.h
    src/core/binary.h
    src/core/buffer.h
    src/core/console.h
    src/core/export.h
    src/core/intrinsics.h
    src/core/logger.h
    src/core/mem_align.h
    src/core/memory.h
    src/core/path.h
    src/core/random.h
    src/core/rect.h
    src/core/scancode.h
    src/core/string.h
    src/core/time_convert.h
    src/core/toml_config.h
    src/core/types.h
    src/core/utils.h
    src/core/vector.h)
set(BM_CORE_SOURCES
    src/core/binary.c
    src/core/buffer.c
    src/core/hashmap.c
    src/core/logger.c
    src/core/mem_align.c
    src/core/memory.c
    src/core/path.c
    src/core/string.c
    src/core/toml_config.c
    src/core/utils.c)

# math
set(BM_MATH_HEADERS
    src/math/types.h
    src/math/utils.h
    src/math/bounds.h
    src/math/vec2.h
    src/math/vec3.h
    src/math/vec4.h
    src/math/mat4.h)

set(BM_MATH_SOURCES
    src/math/mat4.c)

# platform
set(BM_PLATFORM_HEADERS
    src/platform/gamepad.h
    src/platform/keyboard.h
    src/platform/keyboard-scancode.h
    src/platform/mouse.h
    src/platform/platform.h
    src/platform/utf8.h)
set(BM_PLATFORM_SOURCES
    src/platform/platform.c
    src/platform/utf8.c)

# gfx
set(BM_GFX_HEADERS
    src/gfx/enums.h
    src/gfx/gfx.h
    src/gfx/camera.h
    src/gfx/scene.h)
set(BM_GFX_SOURCES
    src/gfx/gfx.c
    src/gfx/camera.c
    src/gfx/scene.c)

set(BM_MEDIA_HEADERS
    src/media/enums.h
    src/media/image.h
    src/media/targa.h)
set(BM_MEDIA_SOURCES
    src/media/image.c)

# gui
set(BM_GUI_HEADERS
    src/gui/gui.h)
set(BM_GUI_SOURCES
    src/gui/gui.c)

# game
set(BM_GAME_HEADERS
    src/asset.h
    # src/audio.h
    src/command.h
    # src/engine.h
    # src/entity.h
    # src/font.h
    src/input.h
    # src/render.h
    # src/resource.h
    # src/sprite.h
)
set(BM_GAME_SOURCES
    src/asset.c
    # src/audio.c
    src/command.c
    # src/engine.c
    # src/entity.c
    # src/font.c
    src/input.c
    src/main.c
    # src/render.c
    # src/resource.c
    # src/sprite.c
)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(TARGET_COMPILE_DEFS
        -DBM_WINDOWS
        -DUNICODE
        -D_UNICODE
        -D_CRT_SECURE_NO_WARNINGS)
        # -DSDL_AUDIODRIVER=dsound)
    set(BM_PLATFORM_HEADERS
        ${BM_PLATFORM_HEADERS}
        src/platform/keyboard-vkey-win32.h)
    set(BM_PLATFORM_SOURCES
        ${BM_PLATFORM_SOURCES}
        src/platform/platform-win32.c)

    set(BM_GFX_HEADERS
        ${BM_GFX_HEADERS}
        src/gfx/dxguids.h
        src/gfx/win32_types.h
        src/gfx/gfx_d3d11.h
        src/gfx/gfx_d3d11_input_layout.h)
    set(BM_GFX_SOURCES
        ${BM_GFX_SOURCES}
        src/gfx/gfx_d3d11.c)
    set(BM_GUI_SOURCES
        ${BM_GUI_SOURCES}
        src/gui/gui_win32.c)

    if (BM_BUILD_32BIT)
        set(TARGET_LINK_DIRS
            deps/SDL2/lib/x86)
        set(BM_ARCH "x86")
    else()
        set(TARGET_LINK_DIRS
            deps/SDL2/lib/x64)
        set(BM_ARCH "x64")
    endif()

    set(TARGET_LINK_LIBS
        # SDL2main.lib
        # SDL2.lib
        # SDL2_image.lib
        # SDL2_mixer.lib
        kernel32)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(TARGET_COMPILE_DEFS
        -DBM_DARWIN)
    set(BM_PLATFORM_SOURCES
        ${BM_PLATFORM_SOURCES}
        src/platform/platform-darwin.c
        src/platform/platform-posix.c)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(TARGET_COMPILE_DEFS
        -DBM_LINUX)
endif()

set(BM_TARGET_SOURCES
    ${MURMURHASH2_SOURCES}
    ${TOMLC99_SOURCES}
    ${BM_CORE_HEADERS}
    ${BM_CORE_SOURCES}
    ${BM_MATH_HEADERS}
    ${BM_MATH_SOURCES}
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

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(bulletmind WIN32 ${BM_TARGET_SOURCES})
    # set_target_properties(bulletmind PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
    set(TARGET_LINK_OPTS ${TARGET_LINK_OPTS} "/SUBSYSTEM:CONSOLE")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_library(SDL2_LIBRARY NAME SDL2 REQUIRED)
    find_library(SDL2_MIXER_LIBRARY NAME SDL2_mixer REQUIRED)
    find_library(SDL2_IMAGE_LIBRARY NAME SDL2_image REQUIRED)

    set(SDL2_INCLUDE_DIR /Library/Frameworks/SDL2.framework/Headers)
    set(SDL2_MIXER_INCLUDE_DIR /Library/Frameworks/SDL2_mixer.framework/Headers)
    set(SDL2_IMAGE_INCLUDE_DIR /Library/Frameworks/SDL2_image.framework/Headers)

    set(TARGET_INCLUDE_DIRS
        ${TARGET_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIR}
        ${SDL2_MIXER_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR})
    set(TARGET_LINK_LIBS
        ${TARGET_LINK_LIBS}
        ${SDL2_LIBRARY}
        ${SDL2_MIXER_LIBRARY}
        ${SDL2_IMAGE_LIBRARY})
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


# set(LIBVV_TARGET_SOURCES
#     ${MURMURHASH2_SOURCES}
#     ${BM_CORE_HEADERS}
#     ${BM_CORE_SOURCES}
#     ${BM_MATH_HEADERS}
#     ${BM_MATH_SOURCES}
#     ${BM_PLATFORM_HEADERS}
#     ${BM_PLATFORM_SOURCES}
#     ${BM_GFX_HEADERS}
#     ${BM_GFX_SOURCES}
#     ${BM_GUI_HEADERS}
#     ${BM_GUI_SOURCES})
# if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
#     set(TARGET_COMPILE_DEFS
#         -DUNICODE
#         -D_UNICODE
#         -D_CRT_SECURE_NO_WARNINGS)
# endif()
# add_library(libvv ${LIBVV_TARGET_SOURCES})
# set_property(TARGET libvv PROPERTY C_STANDARD 11)
# target_compile_definitions(libvv PRIVATE ${TARGET_COMPILE_DEFS})
# target_include_directories(libvv PRIVATE src deps/murmur2)

# add_executable(bulletmind-cpp
#     cpp/main.cpp)
# add_dependencies(bulletmind-cpp libvv)
# target_include_directories(bulletmind-cpp PRIVATE src cpp)
# target_link_libraries(bulletmind-cpp PRIVATE libvv kernel32)
# set_property(TARGET bulletmind-cpp PROPERTY CXX_STANDARD 17)

# post-build commands
# if (CMAKE_SYSTEM_NAME STREQUAL Windows)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libFLAC-8.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/libFLAC-8.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libjpeg-9.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/libjpeg-9.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libmodplug-1.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/libmodplug-1.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libmpg123-0.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/libmpg123-0.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libogg-0.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/libogg-0.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libopus-0.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/libopus-0.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libopusfile-0.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/libopusfile-0.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libpng16-16.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/libpng16-16.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libtiff-5.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/libtiff-5.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libvorbis-0.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/libvorbis-0.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libvorbisfile-3.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/libvorbisfile-3.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libwebp-7.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/libwebp-7.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/SDL2.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/SDL2_image.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2_image.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/SDL2_mixer.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2_mixer.dll)
#     add_custom_command(TARGET bulletmind POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/zlib1.dll
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/zlib1.dll)
# endif()
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
    src/tests.c
    ${MURMURHASH2_SOURCES}
    ${TOMLC99_SOURCES}
    ${BM_CORE_HEADERS}
    ${BM_CORE_SOURCES}
    ${BM_MATH_HEADERS}
    ${BM_MATH_SOURCES}
    ${BM_PLATFORM_HEADERS}
    ${BM_PLATFORM_SOURCES})
set_property(TARGET bm_tests PROPERTY C_STANDARD 11)
target_compile_definitions(bm_tests PRIVATE -D_CRT_SECURE_NO_WARNINGS)
target_include_directories(bm_tests PRIVATE
    ${TARGET_INCLUDE_DIRS}
    deps/tau)
target_link_directories(bm_tests PRIVATE ${TARGET_LINK_DIRS})
target_link_libraries(bm_tests PRIVATE ${TARGET_LINK_LIBS})
target_link_options(bm_tests PRIVATE ${TARGET_LINK_OPTS})
