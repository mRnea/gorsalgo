#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <random>
#include <math.h>
#include "visualize.h"
#include "graph.h"

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Renderer* gRenderer = NULL;

void DrawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
   const int32_t diameter = (radius * 2);

   int32_t x = (radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - diameter);

   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

TTF_Font *font;
bool init() {
    // returns true for successful initialization
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Create window
    gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        fprintf(stderr, "Window could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }
    
    // Get window surface
    // gScreenSurface = SDL_GetWindowSurface(gWindow);

    //Create renderer for window
    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
    if( gRenderer == NULL ){
        fprintf(stderr, "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return false;
    }

    if (TTF_Init()){
        fprintf(stderr, "Could not init ttf. SDL Error: %s", SDL_GetError());
        return false;
    }
    char font_path[] = "./resource/JetBrainsMono-Bold.ttf";
    font = TTF_OpenFont(font_path, 24);
    if (font == NULL) {
        fprintf(stderr, "error: font not found\n");
        return false;
        // exit(EXIT_FAILURE);
    }
    //Initialize renderer color
    //SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0xFF ); // Black

    
    //Initialize PNG loading
    // int imgFlags = IMG_INIT_PNG;
    // if( !( IMG_Init( imgFlags ) & imgFlags )) {
    //     printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
    //     success = false;
    // }

    return true;
}

void close() {
    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    TTF_Quit();

    // Quit SDL subsystems
    SDL_Quit();
}


double distance(double x1, double y1, double x2, double y2){
    return sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
}

size_t select_pgraph(pgraph_t* pg, int mouse_x, int mouse_y){
    // Returns the size of pgraph or
    // if the mouse is in a node returns its index
    for (size_t i = 0; i < pg->size; i++){
        if (distance(mouse_x, mouse_y, pg->vertices[i].x, pg->vertices[i].y) < pg->vertices[i].r){
            return i;
        }
    }
    return pg->size;
}

pvertex_t* grabbed_vertex;
pvertex_t* maybeGrabVertex(pgraph_t* pg, int x, int y){
    int vertex_index = select_pgraph(pg, x, y);
    if (vertex_index != pg->size){
        grabbed_vertex = &pg->vertices[vertex_index];
        return grabbed_vertex;
    }
    else {
        grabbed_vertex = NULL;
        return grabbed_vertex;
    }
}

double pvertex_distance(pvertex_t p1, pvertex_t p2){
    return distance(p1.x, p1.y, p2.x, p2.y);
}

char pvertex_clash(pvertex_t* vertices, int index){
    // Returns 1 if i-th vertex collides with any vertices [0, i)
    pvertex_t p1;
    pvertex_t p2 = vertices[index];
    for (int i = 0; i < index; i++){
        p1 = vertices[i];
        if (pvertex_distance(p1, p2) < (p1.r + p2.r)){
            return 1;
        }
    }
    return 0;
}

pgraph_t* graph_to_pgraph(graph_t* graph){
    char number_buffer[16];
    pvertex_t* vertices = (pvertex_t*) calloc(graph->size, sizeof(pvertex_t));
    for (int i = 0; i < graph->size; i++){
        do {
            vertices[i].x = 20 + rand() % (SCREEN_WIDTH - 40);
            vertices[i].y = 20 + rand() % (SCREEN_HEIGHT - 40);
            vertices[i].r = 10;
        } while(pvertex_clash(vertices, i));
        sprintf(number_buffer, "%d", i);
        get_text_and_rect(gRenderer, number_buffer, font, &vertices[i]);
    }
    pgraph_t* pg = (pgraph_t*) calloc(1, sizeof(pgraph_t));
    pg->vertices = vertices;
    pg->size = graph->size;
    return pg;
}

void render_pvertex(pvertex_t vertex){
    DrawCircle(gRenderer, vertex.x, vertex.y, vertex.r);
    SDL_RenderCopy(gRenderer, vertex.name_texture, NULL, &vertex.name_rect);
}

void render_pgraph(pgraph_t* pgraph){
    for (size_t i = 0; i < pgraph->size; i++){
        render_pvertex(pgraph->vertices[i]);
    }
}

void render_edge(pvertex_t v1, pvertex_t v2){
    SDL_RenderDrawLine(gRenderer, v1.x, v1.y, v2.x, v2.y);
}

void render_edges(graph_t* graph, pgraph_t* pgraph){
    int dim = graph->size;
    for (int i = 0; i < dim; i++){
        for (int j = i; j < dim; j++){
            if (graph->adj_matrix[i * dim + j] == 1){
                render_edge(pgraph->vertices[i], pgraph->vertices[j]);
            }
        }
    }
}

void delete_pgraph(pgraph_t** pg){
    for (int i = 0; i < (*pg)->size; i++){
        SDL_DestroyTexture((*pg)->vertices[i].name_texture);
    }
    free((*pg)->vertices);
    free(*pg);
    *pg = NULL;
}
// double slope = (v1.y - v2.y) / (v1.x - v2.x);
// double dist = distance(v1.x, v1.y, v2.x, v2.y);
// double r1_ratio = v1.r / dist;
// double r2_ratio = v2.r / dist;

/*
- x, y: upper left corner.
- texture, rect: outputs.
*/
void get_text_and_rect(SDL_Renderer *renderer, char *text, TTF_Font *font, pvertex_t* pg) {
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    pg->name_texture = SDL_CreateTextureFromSurface(renderer, surface);
    pg->name_rect.w = surface->w;
    pg->name_rect.h = surface->h;
    SDL_FreeSurface(surface);

    pg->name_rect.x = pg->x - pg->name_rect.w / 2;
    pg->name_rect.y = pg->y - pg->r - pg->name_rect.h - 5 ;
}

void move_pvertex(pvertex_t* p, int x, int y){
    p->x += x;
    p->y += y;
    p->name_rect.x += x;
    p->name_rect.y += y;
}
