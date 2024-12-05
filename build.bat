ECHO OFF
 g++ .\src\graph.cpp .\src\visualize.cpp .\src\main.cpp -I.\sdl\include\SDL2\ -I.\sdl-ttf\include\SDL2\ -L.\sdl\lib\ -L.\sdl-ttf\lib\ -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -o main
 rem -Wl,-subsystem,windows
