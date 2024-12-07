#include "color.h"
#include <random>

const int RANDOM_COLOR = -1;
enum Color DEFAULT_COLOR = WHITE;

enum Color random_color(){
    return (enum Color) (1 + rand() % (COLOR_COUNT - 1));
}

enum Color next_color_wrap(int i){
    return (enum Color) (i % (COLOR_COUNT - 1) + 1);
}

enum Color next_color(int i){
    return (enum Color) (i + 1);
}
