#ifndef BACKTRACKING_HEADER
#define BACKTRACKING_HEADER

#include "../src/graph.h"
#include "../src/visualize.h"

void set_vertex_color(app_t* app, int i, enum Color color);
bool can_color(graph_t* g, int index, enum Color color);
void backtracking(app_t* app, int i);
void color_backtrack(app_t* app);

#endif


