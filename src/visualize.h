#ifdef __linux__ 
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

#ifdef _WIN64
#include <SDL.h>
#include <SDL_ttf.h>
#endif

#include <stdio.h>
#include <string.h>
#include <random>
#include <math.h>
#include "graph.h"
#include "color.h"

#ifndef VISUALIZE_HEADER
#define VISUALIZE_HEADER

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

 // The window we'll be rendering to
extern SDL_Window *gWindow;

 // The surface contained by the window
extern SDL_Surface *gScreenSurface;

//The window renderer
extern SDL_Renderer* gRenderer;

extern TTF_Font *font;
extern char coord_file_name[200];
 // Starts up SDL and creates window
bool init();

// Frees media and shuts down SDL
void close();

void set_render_color(SDL_Renderer* renderer, enum Color color);

void DrawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius);

extern int DEFAULT_RADIUS;
typedef struct physical_vertex_t {
    int x;
    int y;
    int r;
    int i;
    SDL_Texture* name_texture;
    SDL_Rect name_rect;
    enum Color color;
} pvertex_t;

typedef struct physical_edge_t {
    pvertex_t* v1;
    pvertex_t* v2;
} pedge_t;

typedef struct physical_graph_t {
    int vertex_count;
    int max_vertex;
    pvertex_t* vertices;
    int edge_count;
    int max_edge;
    pedge_t* edges;
} pgraph_t;

typedef struct history_slice_t {
    int vi;
    int i;
    int is_solution;
    enum Color from;
    enum Color to;
    history_slice_t* prev;
    history_slice_t* next;
} hist_slice_t;

typedef struct color_history_t {
    hist_slice_t* head;
    hist_slice_t* tail;
    hist_slice_t* current;
    int max_sol;
} hist_t;

typedef struct application_data_t {
    bool quit;
    int mouse_x;
    int mouse_y;
    graph_t* graph;
    pgraph_t* pgraph;
    pvertex_t* gv; // grabbed vertex
    hist_t* hist;
    SDL_Texture* iter_texture;
    SDL_Rect iter_rect;
} app_t;

void add_slice(hist_t* hist, int i, enum Color from, enum Color to, int sol);
void color_vertex(app_t* app, int i, enum Color color);
void delete_hist(hist_t** hist);
void new_hist(app_t* app);
int go_next_slice(app_t* app);
int go_prev_slice(app_t* app);
void go_next_solution(app_t* app);
void go_prev_solution(app_t* app);
void find_max_solution(app_t* app);
void print_history(app_t* app);
void set_app_iter_texture(app_t* app, SDL_Renderer* renderer,TTF_Font* font);

double distance(double x1, double y1, double x2, double y2);
double pvertex_distance(pvertex_t p1, pvertex_t p2);

extern pvertex_t* grabbed_vertex;
pvertex_t* maybeGrabVertex(pgraph_t* pg, int x, int y);
size_t select_pgraph(pgraph_t* pg, int x, int y);

pgraph_t* graph_to_pgraph(graph_t* graph, int color);
graph_t* pgraph_to_graph(pgraph_t* pgraph);
char* get_coord_file(char* file_name);
void autoload_coordinates(app_t* app, char* file_name);
void save_graph(graph_t* graph, char* file_name);
void load_pgraph_coord(pgraph_t* pg, char* file_name);
void print_pgraph_coord(pgraph_t* pg, FILE* stream);
void save_pgraph_coord(pgraph_t* pg, char* file_name);

char pvertex_clash(pvertex_t* vertices, int index);

void render_pvertex(pvertex_t vertex);
void render_pvertices(pgraph_t* pgraph);
void render_pedge(pedge_t edge);
void render_psuedo_pedge(pvertex_t* pv, int x, int y);
void render_pedges(pgraph_t* pgraph);
void render_pgraph(pgraph_t* pgraph);
// void render_edge(pvertex_t v1, pvertex_t v2);
// void render_edges(graph_t* graph, pgraph_t* pgraph);

void delete_pvertex_texture(pvertex_t* pv);
void delete_pgraph(pgraph_t** pg);

void get_text_and_rect(SDL_Renderer *renderer, char *text, TTF_Font *font, pvertex_t* pg);
void pvertex_radius_adjust(pvertex_t* pv, int r);
void pgraph_radius_adjust(pgraph_t* pg, int r);
void move_pvertex(pvertex_t* p, int x, int y);

// graph_area_t graph_area;

// typedef struct graph_area_t {
//     SDL2_Point p1;
// } graph_area_t;

extern int GA_OFFSET;
extern SDL_Point graph_area[5];
void render_graph_area(SDL_Point area[5]);


// Builder functions
pgraph_t* make_empty_pgraph(int max_vertex, int max_edge);

pvertex_t* add_pvertex(pgraph_t* pg, int x, int y, int r, int color);
void remove_pvertex(pgraph_t *pg);

void add_pedge(pgraph_t* pg, pvertex_t* v1, pvertex_t* v2);
void remove_pedge(pgraph_t* pg);

void print_pgraph(pgraph_t* pg);

extern char builder_flags;
enum Builder_states {
                     CAN_ADD_VERTEX     = 1,
                     ADD_VERTEX_CONFIRM = 2,
                     VERTEX_OP          = CAN_ADD_VERTEX | ADD_VERTEX_CONFIRM,
                     CAN_ADD_EDGE       = 4,
                     ADD_EDGE_SELECT1   = 8,
                     ADD_EDGE_SELECT2   = 16,
                     EDGE_OP            = CAN_ADD_EDGE | ADD_EDGE_SELECT1 | ADD_EDGE_SELECT2,
                     CAN_ADD_ANY        = CAN_ADD_VERTEX | CAN_ADD_EDGE,
                     ADDING_ANY         = ADD_VERTEX_CONFIRM | ADD_EDGE_SELECT1 | ADD_EDGE_SELECT2,
                     MOUSE_RECT         = 32,
};

extern pvertex_t* builder_vertices[2];
extern SDL_Rect mouseRect;
extern const int mouse_rect_w;
extern const int mouse_rect_h;
#endif
