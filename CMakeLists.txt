project (bulletmind)

cmake_minimum_required(VERSION 3.18)

option(BM_BUILD_32BIT "build bulletmind as 32-bit" ON)
option(BM_BUILD_64BIT "build bulletmind as 64-bit" OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DBM_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
endif()
if (BM_BUILD_32BIT)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
elseif(BM_BUILD_64BIT)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
endif()

set(BM_INCLUDE_DIRS
    src
    deps/tomlc99
    deps/SDL2/include)

set(TOMLC99_SOURCES
    deps/tomlc99/toml.h
    deps/tomlc99/toml.c)

set(BM_CORE_HEADERS
    src/core/binary.h
    src/core/bitfield.h
    src/core/buffer.h
    src/core/c99defs.h
    src/core/logger.h
    src/core/mem_align.h
    src/core/mem_arena.h
    src/core/rect.h
    src/core/scancodes.h
    src/core/time_convert.h
    src/core/types.h
    src/core/utils.h)

set(BM_CORE_SOURCES
    src/core/binary.c
    src/core/buffer.c
    src/core/logger.c
    src/core/mem_align.c
    src/core/mem_arena.c)

set(BM_MATH_HEADERS
    src/math/math-defs.h
    src/math/vec2.h)

set(BM_PLATFORM_HEADERS
    src/platform/export.h
    src/platform/platform.h
    src/platform/scancode.h
    src/platform/utf8.h)

set(BM_PLATFORM_SOURCES
    src/platform/platform.c
    src/platform/utf8.c)

set(BM_GAME_HEADERS
    src/audio.h
    src/command.h
    src/engine.h
    src/entity.h
    src/font.h
    src/input.h
    src/main.h
    src/render.h
    src/resource.h
    src/sprite.h
    src/toml_config.h)

set(BM_GAME_SOURCES
    src/audio.c
    src/command.c
    src/engine.c
    src/entity.c
    src/font.c
    src/input.c
    src/main.c
    src/render.c
    src/resource.c
    src/sprite.c
    src/toml_config.c)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(
        -DBM_WINDOWS)

    set(BM_PLATFORM_SOURCES
        ${BM_PLATFORM_SOURCES}
        src/platform/platform-win32.c)

    if (BM_BUILD_32BIT)
        set(BM_LIB_DIRS
            deps/SDL2/lib/x86)
        set(BM_ARCH "x86")
    elseif(BM_BUILD_64BIT)
        set(BM_LIB_DIRS
            deps/SDL2/lib/x64)
        set(BM_ARCH "x64")
    endif()

    set(BM_LIBS
        SDL2main.lib
        SDL2.lib
        SDL2_image.lib
        SDL2_mixer.lib
        kernel32)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_definitions(
        -DBM_DARWIN)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(
        -DBM_LINUX)
endif()

set(BM_TARGET_SOURCES
    ${TOMLC99_SOURCES}
    ${BM_CORE_HEADERS}
    ${BM_CORE_SOURCES}
    ${BM_MATH_HEADERS}
    ${BM_PLATFORM_HEADERS}
    ${BM_PLATFORM_SOURCES}
    ${BM_GAME_HEADERS}
    ${BM_GAME_SOURCES})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_executable(bulletmind WIN32 ${BM_TARGET_SOURCES})
    set_target_properties(bulletmind PROPERTIES LINK_FLAGS_DEBUG "-Xlinker /SUBSYSTEM:CONSOLE")
else()
    add_executable(bulletmind ${BM_TARGET_SOURCES})
endif()

set_property(TARGET bulletmind PROPERTY C_STANDARD 11)

target_include_directories(bulletmind PUBLIC ${BM_INCLUDE_DIRS})
target_link_options(bulletmind PUBLIC ${BM_LINK_OPTS})
target_link_directories(bulletmind PUBLIC ${BM_LIB_DIRS})
target_link_libraries(bulletmind PUBLIC ${BM_LIBS})

# copy assets
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)
# copy configs
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/config)
# copy SDL2 DLLs
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libFLAC-8.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/libFLAC-8.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libjpeg-9.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/libjpeg-9.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libmodplug-1.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/libmodplug-1.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libmpg123-0.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/libmpg123-0.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libogg-0.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/libogg-0.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libopus-0.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/libopus-0.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libopusfile-0.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/libopusfile-0.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libpng16-16.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/libpng16-16.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libtiff-5.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/libtiff-5.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libvorbis-0.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/libvorbis-0.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libvorbisfile-3.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/libvorbisfile-3.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/libwebp-7.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/libwebp-7.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/SDL2.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/SDL2_image.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2_image.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/SDL2_mixer.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2_mixer.dll)
add_custom_command(TARGET bulletmind POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/lib/${BM_ARCH}/zlib1.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/zlib1.dll)