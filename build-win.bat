@echo off
mkdir "build/win" 2>NUL
xcopy "SDL2" "build/win"
xcopy /E /I "res" "build/win/res"

g++ main.c icon.o logs_utils\log.c menu\menu.c cta_utils\cta.c mouse_utils\mouse.c C:\cJSON-master\cJSON.c worlds\worlds_utils.c player_utils\player.c screen_utils\screen_utils.c save_utils\save.c -IC:\SDL2\include\SDL2 -LC:\SDL2\lib -g -ggdb -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -o build/win/The_Lost_Project -mwindows