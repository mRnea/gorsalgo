#include "coloring.h"

void set_vertex_color(app_t* app, int i, enum Color color){
  app->graph->colors[i] = color;
  app->pgraph->vertices[i].color = color;
}

void colors_init(app_t* app){
    graph_t* g = app->graph;
    for (int i = 0; i < g->size; i++){
        set_vertex_color(app, i, WHITE);
    }
}

// can the node at index be colored to color ?
bool can_color(graph_t* g, int index, enum Color color){
  for (int i = 0; i < g->size; i++){
    if (g->adj_matrix[g->size * index + i] > 0 && color == g->colors[i]){
      return false;
    }
  }
  printf("index : %d, color : %d -> %d\n", index, g->colors[index], color);
  return true;
}

void backtracking(app_t* app, int i){
  printf("i : %d\n", i);
  graph_t* g = app->graph;
  for (enum Color color = WHITE; color < COLOR_COUNT; color = next_color(color)){
    if (can_color(g, i, color)){
      set_vertex_color(app, i, color);
      if (i + 1 < g->size){
        backtracking(app, i + 1);
      }
    }
  }
}

void color_backtrack(app_t* app){
    colors_init(app);
    backtracking(app, 0);
}


// return number of colors in graph
int greedy(app_t* app){
    graph_t* g = app->graph;
    int max = 0;
    for (int i = 0; i < g->size; i++){
        enum Color color = WHITE;
        for (; color < COLOR_COUNT; color = next_color(color)){
            if (can_color(g, i, color)){
                set_vertex_color(app, i, color);
                if (color > max){
                    max = color;
                }
                break;
            }
        }
    }
    return max ;
}

int color_greedy(app_t* app){
    colors_init(app);
    return greedy(app);
}
