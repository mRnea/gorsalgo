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

void DrawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius);

typedef struct physical_vertex_t {
    int x;
    int y;
    int r;
    SDL_Texture* name_texture;
    SDL_Rect name_rect;
} pvertex_t;

typedef struct pgraph_t {
    int size;
    pvertex_t* vertices;
} pgraph_t;

double distance(double x1, double y1, double x2, double y2);
double pvertex_distance(pvertex_t p1, pvertex_t p2);

/* extern pvertex_t vertices[100]; */
/* extern size_t vertex_count; */
/* void addVertex(int x, int y, int r); */
/* void renderVertices(); */
void renderEdge(pvertex_t v1, pvertex_t v2);

extern pvertex_t* grabbed_vertex;
pvertex_t* maybeGrabVertex(pgraph_t* pg, int x, int y);
size_t checkMouseRegion(pgraph_t* pg, int x, int y);


pgraph_t* graph_to_pgraph(graph_t* graph);
char pvertex_clash(pvertex_t* vertices, int index);
void render_pvertex(pvertex_t vertex);

void render_pgraph(pgraph_t* pgraph);
void render_edges(graph_t* graph, pgraph_t* pgraph);
void delete_pgraph(pgraph_t** pg);

void get_text_and_rect(SDL_Renderer *renderer, char *text, TTF_Font *font, pvertex_t* pg);
void move_pvertex(pvertex_t* p, int x, int y);
#endif
