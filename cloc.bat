cloc.exe libmind game

@echo off
echo ================================================================================= >> history.txt
echo %DATE% >> history.txt
echo ================================================================================= >> history.txt

cloc.exe libmind game >> history.txt

pause
