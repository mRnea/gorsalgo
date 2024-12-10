#include "coloring.h"

enum color_algos current_algorithm;
int max_color;
int sol_counter;

void set_vertex_color(app_t* app, int i, enum Color color){
    if (i + 1 == app->graph->size && color != WHITE){
        add_slice(app->hist, i, app->graph->colors[i], color, ++sol_counter);
    } else {
        add_slice(app->hist, i, app->graph->colors[i], color, 0);
    }
    app->graph->colors[i] = color;
}

void colors_init(app_t* app){
    graph_t* g = app->graph;
    for (int i = 0; i < g->size; i++){
        color_vertex(app, i, WHITE);
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
  enum Color color = next_color(WHITE);
  for (; color < max_color; color = next_color(color)){
    if (can_color(g, i, color)){
      enum Color prev = g->colors[i];
      set_vertex_color(app, i, color);
      if (i + 1 < g->size){
        backtracking(app, i + 1);
      }
      set_vertex_color(app, i, prev);
    }
  }
}

void color_backtrack(app_t* app, int max){
    if (app->graph){
        colors_init(app);
        delete_hist(&app->hist);
        new_hist(app);
        current_algorithm = BACKTRACK;
        max_color = max > 0 && max + 2 <= COLOR_COUNT ? max + 2 : COLOR_COUNT;
        sol_counter = 0;
        backtracking(app, 0);
        find_max_solution(app);
    } else {
        printf("Empty graph can't be solved.\n");
    }
}


// return number of colors in graph
int greedy(app_t* app){
    graph_t* g = app->graph;
    int max = 0;
    for (int i = 0; i < g->size; i++){
        enum Color color = next_color(WHITE);
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
    return max - 1;
}

int color_greedy(app_t* app){
    if (app->graph){
        colors_init(app);
        delete_hist(&app->hist);
        new_hist(app);
        current_algorithm = GREEDY;
        sol_counter = 0;
        // max_color = max > 0 ? max : COLOR_COUNT;
        return greedy(app);
    } else {
        printf("Empty graph can't be solved.\n");
        return 0;
    }
}
