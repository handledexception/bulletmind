project (Bulletmind)

option(BM_BUILD_STATIC "Build libmind static library?" ON)
option(BM_BUILD_SHARED "Build libmind shared library?" OFF)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND TARGET_COMPILE_DEFS -DBM_DEBUG)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Od /Zi")
    endif()
    if (BM_BUILD_32BIT)
        list(APPEND TARGET_LINK_OPTS "/MACHINE:X86")
    else()
        list(APPEND TARGET_LINK_OPTS "/MACHINE:X64")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-int-to-pointer-cast")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    endif()
    if (BM_BUILD_32BIT)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
    endif()
endif()

list(APPEND TARGET_INCLUDE_DIRS
    ${ROOT_DIR}
    ${GAME_DIR}
    ${LIBMIND_DIR}
    ../thirdparty
    ../thirdparty/tomlc99
)

if (BM_USE_CIMGUI)
    list(APPEND TARGET_COMPILE_DEFS
        -DBM_USE_CIMGUI
        -DCIMGUI_DEFINE_ENUMS_AND_STRUCTS
    )
    list(APPEND TARGET_INCLUDE_DIRS
        ../thirdparty/cimgui
        ../thirdparty/cimgui/generator/output
    )
    list(APPEND TARGET_LINK_LIBS cimgui)
endif()
if (BM_USE_SDL2)
    list(APPEND TARGET_COMPILE_DEFS
        -DBM_USE_SDL2
    )
    list(APPEND TARGET_INCLUDE_DIRS
        ../thirdparty/SDL/include
    )
    if (BM_BUILD_STATIC)
        list(APPEND TARGET_LINK_LIBS
            SDL2::SDL2-static
            SDL2_image::SDL2_image-static
            SDL2_mixer::SDL2_mixer-static
        )
    elseif()
        list(APPEND TARGET_LINK_LIBS
            SDL2::SDL2
            SDL2_image::SDL2_image
            SDL2_mixer::SDL2_mixer
        )
    endif()
endif()

# murmur2
set(MURMURHASH2_SOURCES
    ../thirdparty/murmur2/MurmurHash2.h
    ../thirdparty/murmur2/MurmurHash2.c
)
list(APPEND TARGET_INCLUDE_DIRS ../thirdparty/murmur2)

# tomlc99
set(TOMLC99_SOURCES
    ../thirdparty/tomlc99/toml.h
    ../thirdparty/tomlc99/toml.c
)

# core
set(BM_CORE_HEADERS
    core/algorithm.h
    core/binary.h
    core/buffer.h
    core/export.h
    core/file_io.h
    core/intrinsics.h
    core/logger.h
    core/memory.h
    core/path.h
    core/random.h
    core/rect.h
    core/scancode.h
    core/string.h
    core/time_convert.h
    core/timer.h
    core/toml_config.h
    core/types.h
    core/utils.h
    core/vector.h
)
set(BM_CORE_SOURCES
    core/binary.c
    core/buffer.c
    core/file_io.c
    core/hashmap.c
    core/logger.c
    core/memory.c
    core/path.c
    core/string.c
    core/timer.c
    core/toml_config.c
    core/utils.c
)

# math
set(BM_MATH_HEADERS
    math/types.h
    math/utils.h
    math/bounds.h
    math/vec2.h
    math/vec3.h
    math/vec4.h
    math/mat4.h
    math/quat.h
)

# platform
set(BM_PLATFORM_HEADERS
    platform/console.h
    platform/gamepad.h
    platform/input.h
    platform/keyboard.h
    platform/keyboard-scancode.h
    platform/mouse.h
    platform/platform.h
    platform/utf8.h
)
set(BM_PLATFORM_SOURCES
    platform/input.c
    platform/platform.c
    platform/utf8.c
)

# gfx
set(BM_GFX_HEADERS
    gfx/enums.h
    gfx/gfx.h
    gfx/camera.h
)
set(BM_GFX_SOURCES
    gfx/gfx.c
    gfx/camera.c
    gfx/sprite.c
)

set(BM_MEDIA_HEADERS
    media/enums.h
    media/image.h
    media/targa.h
)
set(BM_MEDIA_SOURCES
    media/image.c
)

# gui
set(BM_GUI_HEADERS
    gui/gui.h
)
set(BM_GUI_SOURCES
    gui/gui.c
)
if (BM_USE_SDL2)
    list(APPEND BM_GUI_SOURCES gui/gui_sdl2.c)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND TARGET_COMPILE_DEFS
        -DBM_WINDOWS
        -DUNICODE
        -D_UNICODE
        -D_CRT_SECURE_NO_WARNINGS
    )
    if (BM_USE_CIMGUI)
        list(APPEND TARGET_COMPILE_DEFS
            -DCIMGUI_USE_WIN32
            -DCIMGUI_USE_DX11
        )
        if (BM_USE_SDL2)
            list(APPEND TARGET_COMPILE_DEFS
                -DCIMGUI_USE_SDL2
            )
        endif()
    endif()

    list(APPEND BM_PLATFORM_HEADERS
        platform/keyboard-vkey-win32.h
    )
    list(APPEND BM_PLATFORM_SOURCES
        platform/platform-win32.c
    )

    list(APPEND BM_GFX_HEADERS
        gfx/dxguids.h
        gfx/win32_types.h
        gfx/gfx_d3d11.h
        gfx/gfx_d3d11_input_layout.h
    )
    list(APPEND BM_GFX_SOURCES
        gfx/gfx_d3d11.c
    )
    list(APPEND BM_GUI_SOURCES
        gui/gui_win32.c
    )

    list(APPEND TARGET_LINK_LIBS
        kernel32
        DbgHelp
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND TARGET_COMPILE_DEFS
        -DBM_DARWIN
    )
    list(APPEND BM_PLATFORM_SOURCES
        platform/platform-darwin.c
        platform/platform-posix.c
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND TARGET_COMPILE_DEFS
        -DBM_LINUX
    )
endif()

set(TARGET_SOURCES
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

if (BM_BUILD_STATIC)
    set(BM_LIB_NAME libmind-static)
endif()

add_library(${BM_LIB_NAME} STATIC ${TARGET_SOURCES})
add_dependencies(${BM_LIB_NAME} SDL2::SDL2 SDL2_mixer SDL2_image cimgui)
target_compile_definitions(${BM_LIB_NAME} PRIVATE ${TARGET_COMPILE_DEFS})
target_include_directories(${BM_LIB_NAME} PRIVATE ${TARGET_INCLUDE_DIRS})
target_include_directories(${BM_LIB_NAME} INTERFACE ${TARGET_INCLUDE_DIRS})
target_link_libraries(${BM_LIB_NAME} PRIVATE ${TARGET_LINK_LIBS})
target_link_libraries(${BM_LIB_NAME} INTERFACE ${TARGET_LINK_LIBS})
add_library(Bulletmind::Mind ALIAS ${BM_LIB_NAME})
