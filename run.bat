gcc --std=c99 -O1 -Wall buffer.c command.c entity.c font.c imgfile.c input.c system.c timing.c main.c -I.\ -I.\SDL\Include -L.\SDL\lib -lmingw32 -lSDL2main -lSDL2 -o .\x86\prog.exe
.\x86\prog.exe