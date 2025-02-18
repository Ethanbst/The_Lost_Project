@echo off
mkdir "build/win" 2>NUL
xcopy "SDL2" "build/win"
xcopy /E /I "res" "build/win/res"
xcopy /E /I "music" "build/win/music"

g++ main.c icon.o logs_utils\log.c menu\menu.c cta_utils\cta.c -IC:\SDL2\include\SDL2 -LC:\SDL2\lib -g -ggdb -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -o build/win/The_Lost_Project