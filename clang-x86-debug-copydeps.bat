SET BUILD_OUT_PATH=.\out\windows\clang-x86-debug
SET SDL_LIB_PATH_X86=.\SDL2\lib\x86
SET ASSETS_PATH=.\assets
xcopy /Y %SDL_LIB_PATH_X86%\SDL2.lib %BUILD_OUT_PATH%\
xcopy /Y %SDL_LIB_PATH_X86%\SDL2.dll %BUILD_OUT_PATH%\
xcopy /Y %SDL_LIB_PATH_X86%\SDL2main.lib %BUILD_OUT_PATH%\
xcopy /Y %ASSETS_PATH%\7px_font.tga %BUILD_OUT_PATH%\