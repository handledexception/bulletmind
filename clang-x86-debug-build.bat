@echo off

SET BUILD_OUT_PATH=.\out\windows\clang-x86-debug
SET SDL_LIB_PATH_X86=.\SDL2\lib\x86
SET ASSETS_PATH=.\assets

REM clean and build
del /Q %BUILD_OUT_PATH%\*
clang --std=c11 -g -O0 -Wall -m32 .\src\buffer.c .\src\command.c .\src\entity.c .\src\font.c .\src\imgfile.c .\src\input.c .\src\render.c .\src\system.c .\src\timing.c .\src\main.c .\src\memarena.c -I.\src -I.\SDL2\include -L.\SDL2\lib\x86 -lkernel32 -lSDL2main -lSDL2 -Xlinker /SUBSYSTEM:console -o %BUILD_OUT_PATH%\bmind.exe

REM copy deps and assets
xcopy /Y %SDL_LIB_PATH_X86%\SDL2.lib %BUILD_OUT_PATH%\
xcopy /Y %SDL_LIB_PATH_X86%\SDL2.dll %BUILD_OUT_PATH%\
xcopy /Y %SDL_LIB_PATH_X86%\SDL2main.lib %BUILD_OUT_PATH%\
xcopy /Y %ASSETS_PATH%\font_7px.tga %BUILD_OUT_PATH%\
xcopy /Y %ASSETS_PATH%\bullet_ps.tga %BUILD_OUT_PATH%\