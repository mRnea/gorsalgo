#ifndef COLORS_HEADER
#define COLORS_HEADER

enum Color {
            BLACK,
            WHITE,
            RED,
            GREEN,
            BLUE,
            COLOR_COUNT
};

extern enum Color DEFAULT_COLOR;
extern const int RANDOM_COLOR;

enum Color random_color();
enum Color next_color_wrap(int i);
enum Color next_color(int i);

#endif
