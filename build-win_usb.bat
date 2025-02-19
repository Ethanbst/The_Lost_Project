@echo off
mkdir "D:/win" 2>NUL

xcopy "SDL2" "D:/win"
xcopy /E /I "res" "D:/win/res"
xcopy /E /I "music" "D:/win/music"

g++ main.c icon.o logs_utils\log.c menu\menu.c cta_utils\cta.c mouse_utils\mouse.c C:\cJSON-master\cJSON.c -IC:\SDL2\include\SDL2 -LC:\SDL2\lib -g -ggdb -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -o D:/win/The_Lost_Project