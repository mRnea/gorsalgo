#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <random>
#include <math.h>
#include "graph.h"

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
 // Starts up SDL and creates window
bool init();

// Frees media and shuts down SDL
void close();

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
void set_render_color(SDL_Renderer* renderer, enum Color color);
enum Color random_color();
enum Color next_color(int i);

void DrawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius);

extern int DEFAULT_RADIUS;
typedef struct physical_vertex_t {
    int x;
    int y;
    int r;
    SDL_Texture* name_texture;
    SDL_Rect name_rect;
    enum Color color;
} pvertex_t;

typedef struct color_history_t {
    pvertex_t* pvertex;
    enum Color from;
    enum Color to;
} hist_t;

void set_pvertex_color(pvertex_t* pvertex, enum Color new_color, hist_t* hist);
void set_pvertex_color_undo(hist_t* hist);

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

double distance(double x1, double y1, double x2, double y2);
double pvertex_distance(pvertex_t p1, pvertex_t p2);

extern pvertex_t* grabbed_vertex;
pvertex_t* maybeGrabVertex(pgraph_t* pg, int x, int y);
size_t select_pgraph(pgraph_t* pg, int x, int y);

pgraph_t* graph_to_pgraph(graph_t* graph, int color);
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

void add_pvertex(pgraph_t* pg, int x, int y, int r, int color);
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
