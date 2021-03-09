@echo off

SET BUILD_OUT_PATH=.\out\windows\clang-x86-debug
SET SDL_LIB_PATH_X86=.\deps\SDL2\lib\x86
SET ASSETS_PATH=.\assets
set CONFIG_PATH=.\config

ECHO Removing existing build artifacts...
REM clean and build
del /Q %BUILD_OUT_PATH%\*
rmdir /S /Q %BUILD_OUT_PATH%\assets
rmdir /S /Q %BUILD_OUT_PATH%\config
mkdir %BUILD_OUT_PATH%\assets
mkdir %BUILD_OUT_PATH%\config
ECHO.

ECHO Building platform library x86 in %BUILD_OUT_PATH%...
clang --std=c11 -g -O0 -Wall -m32 ^
-D BM_WINDOWS ^
-D BM_DEBUG ^
-c .\src\platform\platform.c ^
-I.\src ^
-o %BUILD_OUT_PATH%\platform.o

ECHO Building platform library x86 in %BUILD_OUT_PATH%...
clang --std=c11 -g -O0 -Wall -m32 ^
-D BM_WINDOWS ^
-D BM_DEBUG ^
-c .\src\platform\platform-win32.c ^
-I.\src ^
-o %BUILD_OUT_PATH%\platform-win32.o

ECHO Building platform library x86 in %BUILD_OUT_PATH%...
clang --std=c11 -g -O0 -Wall -m32 ^
-D BM_WINDOWS ^
-D BM_DEBUG ^
-c .\src\platform\utf8.c ^
-I.\src ^
-o %BUILD_OUT_PATH%\utf8.o

llvm-ar -crv %BUILD_OUT_PATH%\platform.lib %BUILD_OUT_PATH%\platform.o %BUILD_OUT_PATH%\platform-win32.o %BUILD_OUT_PATH%\utf8.o

ECHO Building Bulletmind x86 in %BUILD_OUT_PATH%...
clang --std=c11 -g -O0 -Wall -m32 ^
-D BM_WINDOWS ^
-D BM_DEBUG ^
.\src\binary.c ^
.\src\buffer.c ^
.\src\command.c ^
.\src\entity.c ^
.\src\font.c ^
.\src\sprite.c ^
.\src\input.c ^
.\src\render.c ^
.\src\resource.c ^
.\src\engine.c ^
.\src\toml_config.c ^
.\src\main.c ^
.\src\memalign.c ^
.\src\memarena.c ^
.\deps\tomlc99\toml.c ^
-I.\src -I.\deps\tomlc99 -I.\deps\SDL2\include ^
-L.\deps\SDL2\lib\x86 ^
-l%BUILD_OUT_PATH%\platform ^
-lkernel32 -lSDL2main -lSDL2 ^
-Xlinker /SUBSYSTEM:console ^
-o %BUILD_OUT_PATH%\bmind.exe
ECHO.

REM copy deps and assets
ECHO Copying dependencies...
xcopy /Y %SDL_LIB_PATH_X86%\SDL2.lib %BUILD_OUT_PATH%\
xcopy /Y %SDL_LIB_PATH_X86%\SDL2.dll %BUILD_OUT_PATH%\
xcopy /Y %SDL_LIB_PATH_X86%\SDL2main.lib %BUILD_OUT_PATH%\
ECHO.
ECHO Copying assets...
xcopy /Y %ASSETS_PATH%\ss_player.tga %BUILD_OUT_PATH%\assets\
xcopy /Y %ASSETS_PATH%\ss_player_rgba.tga %BUILD_OUT_PATH%\assets\
xcopy /Y %ASSETS_PATH%\ss_player.toml %BUILD_OUT_PATH%\assets\
xcopy /Y %ASSETS_PATH%\ss_skelly.tga %BUILD_OUT_PATH%\assets\
xcopy /Y %ASSETS_PATH%\ss_skelly.toml %BUILD_OUT_PATH%\assets\
xcopy /Y %ASSETS_PATH%\font_7px.tga %BUILD_OUT_PATH%\assets\
xcopy /Y %ASSETS_PATH%\font_7px_rgba.tga %BUILD_OUT_PATH%\assets\
xcopy /Y %ASSETS_PATH%\bullet_ps.tga %BUILD_OUT_PATH%\assets\
xcopy /Y %ASSETS_PATH%\bullet_rgba.tga %BUILD_OUT_PATH%\assets\
xcopy /Y %CONFIG_PATH%\assets.toml %BUILD_OUT_PATH%\config\
ECHO.
