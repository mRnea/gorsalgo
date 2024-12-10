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
#include "visualize.h"
#include "graph.h"

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
int GA_OFFSET = 15;
int DEFAULT_RADIUS = 10;

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Renderer* gRenderer = NULL;

SDL_Point graph_area[5] = {{ GA_OFFSET, GA_OFFSET },
                           { SCREEN_WIDTH - GA_OFFSET, GA_OFFSET},
                           { SCREEN_WIDTH - GA_OFFSET, SCREEN_HEIGHT - GA_OFFSET},
                           { GA_OFFSET, SCREEN_HEIGHT - GA_OFFSET },
                           { GA_OFFSET, GA_OFFSET }};

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

void draw_filled_circle(SDL_Renderer *renderer, int x, int y, int radius){
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
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
    gWindow = SDL_CreateWindow("Graph Coloring", SDL_WINDOWPOS_UNDEFINED,
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

size_t select_pvertex(pgraph_t* pg, int mouse_x, int mouse_y){
    // Returns the size of pgraph or
    // if the mouse is in a node returns its index
    for (size_t i = 0; i < pg->vertex_count; i++){
        if (distance(mouse_x, mouse_y, pg->vertices[i].x, pg->vertices[i].y) < pg->vertices[i].r){
            return i;
        }
    }
    return pg->vertex_count;
}

pvertex_t* grabbed_vertex;
pvertex_t* maybeGrabVertex(pgraph_t* pg, int x, int y){
    int vertex_index = select_pvertex(pg, x, y);
    if (NULL == pg){
        grabbed_vertex = NULL;
    }
    else if (vertex_index != pg->vertex_count){
        grabbed_vertex = &pg->vertices[vertex_index];
    }
    else {
        grabbed_vertex = NULL;
    }
    return grabbed_vertex;
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

char number_buffer[16];
char* int_to_string(int num){
    sprintf(number_buffer, "%d", num);
    return &number_buffer[0];
}

pgraph_t* graph_to_pgraph(graph_t* graph, int color){
    if (!graph){
        fprintf(stderr, "NULL graph could not be converted to pgraph.\n");
    }
    pvertex_t* vertices = (pvertex_t*) calloc(graph->size, sizeof(pvertex_t));
    char* num_str = NULL;
    int offset = GA_OFFSET + 10;
    for (int i = 0; i < graph->size; i++){
        do {
            vertices[i].x = offset + rand() % (SCREEN_WIDTH - offset * 2);
            vertices[i].y = offset + rand() % (SCREEN_HEIGHT - offset * 2);
            vertices[i].r = DEFAULT_RADIUS;
            if (color == RANDOM_COLOR){
                vertices[i].color = random_color();
            }
            else {
                vertices[i].color = (enum Color) color;
            }
        } while(pvertex_clash(vertices, i));
        vertices[i].i = i;
        num_str = int_to_string(i);
        get_text_and_rect(gRenderer, num_str, font, &vertices[i]);
    }
    pgraph_t* pg = (pgraph_t*) calloc(1, sizeof(pgraph_t));
    pg->vertices = vertices;
    pg->vertex_count = graph->size;
    pg->max_vertex = graph->size;
    
    int edge_count = 0;
    int dim = graph->size;
    for (int i = 0; i < dim; i++){
        for (int j = i; j < dim; j++){
            if (graph->adj_matrix[i * dim + j] == 1){
                edge_count++;
                // render_edge(pgraph->vertices[i], pgraph->vertices[j]);
            }
        }
    }
    pedge_t* edges = (pedge_t*) calloc(edge_count, sizeof(pedge_t));
    int edge_index = 0;
    for (int i = 0; i < dim; i++){
        for (int j = i; j < dim; j++){
            if (graph->adj_matrix[i * dim + j] == 1){
                edges[edge_index].v1 = &vertices[i];
                edges[edge_index].v2 = &vertices[j];
                edge_index++;
            }
        }
    }
    pg->edge_count = edge_count;
    pg->max_edge = edge_count;
    pg->edges = edges;
    return pg;
}

graph_t* pgraph_to_graph(pgraph_t* pgraph){
    int dim = pgraph->vertex_count;
    int* arr = (int*) calloc(dim * dim, sizeof(int));
    graph_t* graph = make_graph(dim, arr);

    for (int i = 0; i < pgraph->edge_count; i++){
        pedge_t* edge = &pgraph->edges[i];
        arr[edge->v1->i * dim + edge->v2->i] = 1;
        arr[edge->v2->i * dim + edge->v1->i] = 1;
    }
        // (graph_t*) calloc(1, sizeof(graph_t));
    
    return graph;
}

char coord_file_name[200];
char* get_coord_file(char* file_name){
    char* coord_file = &coord_file_name[0];
    while (*file_name != '.'){
        *coord_file++ = *file_name++;
    }
    char suffix[7] = ".coord";
    for (int i = 0; i < 7; i++){
        *coord_file++ = suffix[i];
    }
    return coord_file_name;
}

void autoload_coordinates(app_t* app, char* file_name){
    char* coord_file = get_coord_file(file_name);
    if (coord_file){
        load_pgraph_coord(app->pgraph, coord_file);
    }
}

void save_graph(graph_t* graph, char* file_name){
    FILE* f = fopen(file_name, "w");
    if (f){
        print_graph(graph, f);
        fclose(f);
    } else {
        fprintf(stderr, "graph could not be saved to %s\n", file_name);
    }
}

void render_pvertex(pvertex_t vertex){
    set_render_color(gRenderer, vertex.color);
    // DrawCircle(gRenderer, vertex.x, vertex.y, vertex.r);
    draw_filled_circle(gRenderer, vertex.x, vertex.y, vertex.r);
    SDL_RenderCopy(gRenderer, vertex.name_texture, NULL, &vertex.name_rect);
}

void render_pvertices(pgraph_t* pgraph){
    for (size_t i = 0; i < pgraph->vertex_count; i++){
        render_pvertex(pgraph->vertices[i]);
    }
}

void render_pedge(pedge_t edge){
    SDL_RenderDrawLine(gRenderer, edge.v1->x, edge.v1->y,
                       edge.v2->x, edge.v2->y);
}

void render_psuedo_pedge(pvertex_t* pv, int x, int y){
    // Psuedo pedge does not have a 2nd vertex.
    SDL_RenderDrawLine(gRenderer, pv->x, pv->y, x, y);
}

void render_pedges(pgraph_t* pgraph){
    set_render_color(gRenderer, WHITE);
    for (int i = 0; i < pgraph->edge_count; i++){
        render_pedge(pgraph->edges[i]);
    }
}

void render_pgraph(pgraph_t* pgraph){
    render_pedges(pgraph);
    render_pvertices(pgraph);
}


void delete_pvertex_texture(pvertex_t* pv){
    SDL_DestroyTexture(pv->name_texture);
}

void delete_pgraph(pgraph_t** pg){
    pvertex_t* pv;
    if (NULL != *pg){
        for (int i = 0; i < (*pg)->vertex_count; i++){
            // SDL_DestroyTexture((*pg)->vertices[i].name_texture);
            delete_pvertex_texture((*pg)->vertices + i);
            // pv = (*pg)->vertices + i;
            // delete_pvertex(&((*pg)->vertices + i));
        }
        free((*pg)->vertices);
        (*pg)->vertices = NULL;
        free((*pg)->edges);
        (*pg)->edges = NULL;
        free(*pg);
        *pg = NULL;
    }
}
// double slope = (v1.y - v2.y) / (v1.x - v2.x);
// double dist = distance(v1.x, v1.y, v2.x, v2.y);
// double r1_ratio = v1.r / dist;
// double r2_ratio = v2.r / dist;

/*
- x, y: upper left corner.
- texture, rect: outputs.
*/
void get_text_and_rect(SDL_Renderer *renderer, char *text, TTF_Font *font, pvertex_t* pv) {
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    pv->name_texture = SDL_CreateTextureFromSurface(renderer, surface);
    pv->name_rect.w = surface->w;
    pv->name_rect.h = surface->h;
    SDL_FreeSurface(surface);

    pv->name_rect.x = pv->x - pv->name_rect.w / 2;
    pv->name_rect.y = pv->y - pv->r - pv->name_rect.h - 5;
}

void pvertex_radius_adjust(pvertex_t* pv, int r){
    pv->r += r;
    pv->name_rect.y = pv->y - pv->r - pv->name_rect.h - 5;
}

void pgraph_radius_adjust(pgraph_t* pg, int r){
    for (int i = 0; i < pg->vertex_count; i++){
        pvertex_radius_adjust(&pg->vertices[i], r);
    }
}

void move_pvertex(pvertex_t* p, int x, int y){
    // displace pvertex by (x, y)
    int nx = p->x + x;
    int ny = p->y + y;

    if (nx > graph_area[0].x && nx < graph_area[1].x &&
        ny > graph_area[1].y && ny < graph_area[2].y){
        p->x = nx;
        p->y = ny;
        p->name_rect.x += x;
        p->name_rect.y += y;
    }

}

void set_render_color(SDL_Renderer* renderer, enum Color color){
    switch (color){
    case WHITE:
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        break;
    case RED:
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0, 0, 0xFF );
        break;
    case GREEN:
        SDL_SetRenderDrawColor( gRenderer, 0, 0xFF, 0, 0xFF );
        break;
    case BLUE:
        SDL_SetRenderDrawColor( gRenderer, 0, 0, 0xFF, 0xFF );
        break;
    case PURPLE:
        SDL_SetRenderDrawColor( gRenderer, 128, 0, 128, 0xFF );
        break;
    case YELLOW:
        SDL_SetRenderDrawColor( gRenderer, 255, 255, 0, 0xFF );
        break;
    case ORANGE:
        SDL_SetRenderDrawColor( gRenderer, 255, 165, 0, 0xFF );
        break;
    case MAGENTA:
        SDL_SetRenderDrawColor( gRenderer, 255, 0, 255, 0xFF );
        break;
    case BLACK:
    default:
        SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0xFF );
    }
}

void color_vertex(app_t* app, int i, enum Color color){
    app->graph->colors[i] = color;
    app->pgraph->vertices[i].color = color;
}


void delete_hist(hist_t** hist){
    if (*hist){
        hist_slice_t* curr = (*hist)->head;
        hist_slice_t* prev;
        while (curr){
            prev = curr;
            curr = curr->next;
            delete(prev);
        }
        delete(*hist);
    }
    (*hist) = NULL;
}

void new_hist(app_t* app){
    app->hist = (hist_t*) calloc(1, sizeof(hist_t));
    hist_t* h = app->hist;
    h->head = (hist_slice_t*) calloc(1, sizeof(hist_slice_t));
    h->tail = (hist_slice_t*) calloc(1, sizeof(hist_slice_t));
    h->current = h->head;
    h->head->prev = NULL;
    h->head->next = h->tail;
    h->head->i = 0;
    h->tail->next = NULL;
    h->tail->prev = h->head;
    h->tail->i = 1;
}

void add_slice(hist_t* hist, int vi, enum Color from, enum Color to){
    hist_slice_t* slice = (hist_slice_t*) calloc(1, sizeof(hist_slice_t));
    slice->from = from;
    slice->to = to;
    slice->vi = vi;

    slice->i = hist->tail->i;
    hist->tail->i++;

    slice->prev = hist->tail->prev;
    slice->next = hist->tail;

    hist->tail->prev->next = slice;
    hist->tail->prev = slice;
}

bool is_history_boundary(hist_slice_t* s){
    return s->next == NULL || s->prev == NULL;
}

void render_next_slice(app_t* app){
    hist_t* hist = app->hist;
    if (hist){
        hist_slice_t* curr = hist->current;
        if (curr->next && !is_history_boundary(curr->next)){
            hist->current = curr->next;
            curr = hist->current;
            color_vertex(app, curr->vi, curr->to);
            set_app_iter_texture(app, gRenderer, font);
        }
    }
}

void render_prev_slice(app_t* app){
    hist_t* hist = app->hist;
    if (hist){
        hist_slice_t* curr = hist->current;
        if (!is_history_boundary(curr)){
            color_vertex(app, curr->vi, curr->from);
            hist->current = curr->prev;
            curr = hist->current;
            set_app_iter_texture(app, gRenderer, font);
        }
    }
}

void print_history(app_t* app){
    if (app->hist){
        hist_slice_t* curr = app->hist->head->next;
        while (curr->next){
            printf("%d : %d -> %d\n", curr->i, curr->from, curr->to);
            curr = curr->next;
        }
    }
    else {
        printf("No app hist\n");
    }
}

void set_app_iter_texture(app_t* app, SDL_Renderer* renderer,TTF_Font* font){
    if (app->iter_texture){
        SDL_DestroyTexture(app->iter_texture);        
        app->iter_texture = NULL;
    }
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    char text[30]; 
    snprintf(text, 30, "%d/%d", app->hist->current->i, app->hist->tail->i - 1);
    surface = TTF_RenderText_Solid(font, text, textColor);
    app->iter_texture = SDL_CreateTextureFromSurface(renderer, surface);
    app->iter_rect.w = surface->w;
    app->iter_rect.h = surface->h;
    SDL_FreeSurface(surface);

    app->iter_rect.x = 20;
    app->iter_rect.y = 20;
}


void render_graph_area(SDL_Point area[5]){
    set_render_color(gRenderer, WHITE);
    SDL_RenderDrawLines(gRenderer, area, 5);
}

pgraph_t* make_empty_pgraph(int max_vertex, int max_edge){
    pgraph_t* pgraph = (pgraph_t*) calloc(1, sizeof(pgraph_t));
    pgraph->max_vertex = max_vertex;
    pgraph->vertices = (pvertex_t*) calloc(max_vertex, sizeof(pvertex_t));
    pgraph->max_edge = max_edge;
    pgraph->edges = (pedge_t*) calloc(max_edge, sizeof(pedge_t));
    pgraph->vertex_count = 0;
    pgraph->edge_count = 0;
    return pgraph;
}

pvertex_t* add_pvertex(pgraph_t* pg, int x, int y, int r, int color){
    char* num_str = NULL;
    // pvertex_t* new_vertex;
    if (pg->vertex_count < pg->max_vertex){
        pg->vertices[pg->vertex_count].x = x;
        pg->vertices[pg->vertex_count].y = y;
        pg->vertices[pg->vertex_count].r = r;
        pg->vertices[pg->vertex_count].i = pg->vertex_count;
        // pg->vertices[pg->vertex_count].color = color;
        if (color == RANDOM_COLOR){
            pg->vertices[pg->vertex_count].color = random_color();
        }
        else {
            pg->vertices[pg->vertex_count].color = (enum Color) color;
        }
        num_str = int_to_string(pg->vertex_count);
        // new_vertex = ;
        get_text_and_rect(gRenderer, num_str, font, &pg->vertices[pg->vertex_count]);
        pg->vertex_count++;
        return &pg->vertices[pg->vertex_count - 1];
    }
    else {
        fprintf(stderr, "Cannot add new vertex, max vertex count is (%d)\n", pg->max_vertex);
        return NULL;
    }
}

void remove_pvertex(pgraph_t *pg){
    if (pg->vertex_count > 0){
        delete_pvertex_texture(&pg->vertices[pg->vertex_count]);
        pg->vertex_count--;
    }
}

void add_pedge(pgraph_t* pg, pvertex_t* v1, pvertex_t* v2){
    int current = pg->edge_count;
    int max = pg->max_edge;
    if (current < max){
        pg->edges[current].v1 = v1;
        pg->edges[current].v2 = v2;
        pg->edge_count++;
    }
    else {
        fprintf(stderr, "Cannot add new edge, max edge count is (%d)\n", pg->max_edge);
    }
}

void remove_pedge(pgraph_t* pg){
    pg->edge_count--;
}

void print_pgraph(pgraph_t* pg){
    printf("vertices %d/%d\n", pg->vertex_count, pg->max_vertex);
    for (int i = 0; i < pg->vertex_count; i++){
        printf("%d : (%d, %d, %d)\n", i, pg->vertices[i].x, pg->vertices[i].y, pg->vertices[i].r);
    }
}

char builder_flags = CAN_ADD_ANY;
pvertex_t* builder_vertices[2];

const int mouse_rect_w = 8;
const int mouse_rect_h = 8;
SDL_Rect mouseRect = { 0, 0, 8, 8 };

void load_pgraph_coord(pgraph_t* pg, char* file_name){
    FILE* f = fopen(file_name, "r");
    if (!f){
        fprintf(stderr, "file \"%s\" could not be opened", file_name);
        return;
    }
    read_eof = 0;
    int_obj x, y;
    int i = 0;
    pvertex_t* pv = NULL;
    while (!read_eof && i < pg->vertex_count){
        pv = &pg->vertices[i];
        x = read_int(f);
        y = read_int(f);
        if (y.length > 0 && x.length > 0){
            pv->x = x.value;
            pv->y = y.value;
            pv->name_rect.x = pv->x - pv->name_rect.w / 2;
            pv->name_rect.y = pv->y - pv->r - pv->name_rect.h - 5;
        }

        i++;
    }
    fclose(f);
}

void print_pgraph_coord(pgraph_t* pg, FILE* stream){
    pvertex_t* pv = NULL;
    for (int i = 0; i < pg->vertex_count; i++){
        pv = &pg->vertices[i];
        fprintf(stream, "%d %d\n", pv->x, pv->y);
    }
}

void save_pgraph_coord(pgraph_t* pg, char* file_name){
    FILE* f = fopen(file_name, "w");
    if (!f){
        fprintf(stderr, "file \"%s\" could not be opened\n", file_name);
        return;
    }
    print_pgraph_coord(pg, f);
    fprintf(stdout, "coordinates are saved to \"%s\"\n", file_name);
    fclose(f);
}
