#include "backtracking.h"

void set_vertex_color(app_t* app, int i, enum Color color){
  app->graph->colors[i] = color;
  app->pgraph->vertices[i].color = color;
}

// can the node at index be colored to color ?
bool can_color(graph_t* g, int index, enum Color color){
  for (int i = 0; i < g->size; i++){
    if (g->adj_matrix[g->size * index + i] == 1 && color == g->colors[i]){
      return false;
    }
  }
  printf("index : %d, color : %d\n", index, color);
  return true;
}

void backtracking(app_t* app, int i){
  printf("i : %d\n", i);
  graph_t* g = app->graph;
  for (enum Color color = WHITE; color < COLOR_COUNT; color = next_color_no_loop(color)){
    if (can_color(g, i, color)){
      set_vertex_color(app, i, color);
      if (i + 1 < g->size){
        backtracking(app, i + 1);
      }
    }
  }
}

void color_backtrack(app_t* app){
  backtracking(app, 0);
}

