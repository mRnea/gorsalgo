#include "graph.h"
#include "visualize.h"

char* file_name;

void printUsage(){
    printf("Görsel Algoritma\n"
           "\n"
           "-load: İçinde komşuluk matrisi olan dosyayı aç.\n"
           "./main -load graph.txt\n"
           "\n"
           "-builder: Graphı inşa et.\n"
           "v - köşe ekle.\n"
           "e - kenar ekle.\n");
    exit(EXIT_SUCCESS);
}
// comment line
enum Modes {
                  FILE_PROVIDED = 1,
                  BUILDER_MODE  = 2
};

char parse_flags;
char parse_args(int argc, char *args[]){
    char flags = 0;
    if (argc == 1){
        printUsage();
    }
    for (int i = 1; i < argc; i++){
        if (strcmp("-load", args[i]) == 0){
            if (i + 1 < argc){
                file_name = args[++i];
                flags ^= 1;
            }
            else {
                fprintf(stderr, "file not provided\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp("-builder", args[i]) == 0){
            flags ^= 2;
        }
        else if (strcmp("-help", args[i]) == 0){
            printUsage();
        }
        else {
            printUsage();
        }
    }
    parse_flags = flags;
}

char parse_status(enum Modes flag){
    return parse_flags & flag;
}

typedef struct application_data_t {
    bool quit;
    int mouse_x;
    int mouse_y;
    graph_t* graph;
    pgraph_t* pgraph;
    pvertex_t* gv; // grabbed vertex
    hist_t* hist;
} app_t;

void handle_event(SDL_Event* e, app_t* app){
    // User requests quit
    if (e->type == SDL_QUIT) {
        app->quit = true;
    }
    // User presses a key
    else if (e->type == SDL_KEYDOWN) {
        // Select surfaces based on key press
        switch (e->key.keysym.sym) {
        case SDLK_ESCAPE:
            app->quit = true;
            break;
        case SDLK_v:
            switch (builder_flags & VERTEX_OP){
            case CAN_ADD_VERTEX:
                SDL_GetMouseState(&app->mouse_x, &app->mouse_y);
                app->gv = add_pvertex(app->pgraph, app->mouse_x, app->mouse_y, DEFAULT_RADIUS, RANDOM_COLOR);
                if (app->gv){
                    builder_flags = ADD_VERTEX_CONFIRM | MOUSE_RECT;
                }
                break;
            case ADD_VERTEX_CONFIRM:
                remove_pvertex(app->pgraph);
                builder_flags = CAN_ADD_ANY;
                break;
            }
            break;
        case SDLK_e:
            switch (builder_flags & EDGE_OP){
            case CAN_ADD_EDGE:
                builder_flags = ADD_EDGE_SELECT1 | MOUSE_RECT; 
                break;
            case ADD_EDGE_SELECT1:
            case ADD_EDGE_SELECT2:
                builder_flags = CAN_ADD_ANY;
                // remove_pedge(pgraph);
                break;
            }
            break;
        case SDLK_c:
            SDL_GetMouseState(&app->mouse_x, &app->mouse_y);
            app->gv = maybeGrabVertex(app->pgraph, app->mouse_x, app->mouse_y);
            if (app->gv){
                set_pvertex_color(app->gv, next_color(app->gv->color), app->hist);
                app->gv = NULL;
            }
            break;
        case SDLK_u:
            set_pvertex_color_undo(app->hist);
            break;
        case SDLK_x:
            print_pgraph(app->pgraph);
            break;
        case SDLK_g:
            delete_graph(&app->graph);
            app->graph = pgraph_to_graph(app->pgraph);
            if (app->graph){
                print_graph(app->graph, stdout);
            } else {
                fprintf(stderr, "Graph could not be converted.\n");
            }
            break;
        // case SDLK_s:
        //     delete_graph(&app->graph);
        //     break;
        default:
            break;
        }
    }
    else if (e->type == SDL_MOUSEBUTTONDOWN){
        switch (builder_flags & ADDING_ANY){
        case ADD_VERTEX_CONFIRM:
            builder_flags = CAN_ADD_ANY;
            app->gv = NULL;
            break;
        case ADD_EDGE_SELECT1:
            SDL_GetMouseState(&app->mouse_x, &app->mouse_y);
            app->gv = maybeGrabVertex(app->pgraph, app->mouse_x, app->mouse_y);
            if (app->gv){
                builder_vertices[0] = app->gv;
                app->gv = NULL;
                builder_flags = ADD_EDGE_SELECT2 | MOUSE_RECT;
            }
            break;
        case ADD_EDGE_SELECT2:
            SDL_GetMouseState(&app->mouse_x, &app->mouse_y);
            app->gv = maybeGrabVertex(app->pgraph, app->mouse_x, app->mouse_y);
            if (app->gv){
                builder_vertices[1] = app->gv;
                app->gv = NULL;
                add_pedge(app->pgraph, builder_vertices[0], builder_vertices[1]);
                builder_flags = CAN_ADD_ANY;
            }
            break;
        default:
            app->gv = maybeGrabVertex(app->pgraph, e->button.x, e->button.y);
        }
    }
    else if (e->type == SDL_MOUSEBUTTONUP){
        app->gv = NULL;
    }
    else if (e->type == SDL_MOUSEMOTION){
        if (app->gv){
            move_pvertex(app->gv, e->motion.xrel, e->motion.yrel);
        }
    }
}

void render_app(SDL_Renderer* renderer, app_t* app){
    set_render_color(renderer, BLACK);
    SDL_RenderClear(renderer);

    if (app->pgraph){
        render_pgraph(app->pgraph);
        render_graph_area(graph_area);
    }

    if (builder_flags & MOUSE_RECT){
        SDL_GetMouseState(&app->mouse_x, &app->mouse_y);
        mouseRect.x = app->mouse_x - mouse_rect_w / 2;
        mouseRect.y = app->mouse_y - mouse_rect_w / 2;
        SDL_RenderFillRect(renderer, &mouseRect);
    }
    if (builder_flags & ADD_EDGE_SELECT2){
        SDL_GetMouseState(&app->mouse_x, &app->mouse_y);
        render_psuedo_pedge(builder_vertices[0], app->mouse_x, app->mouse_y);
    }

    SDL_RenderPresent(renderer);
}

app_t app = { .quit = false, .graph = NULL, .pgraph = NULL };

int main(int argc, char *args[]) {
    parse_args(argc, args);
    srand(time(NULL));

    if (!init()) {
        printf("Failed to initialize SDL2!\n");
        close();
        return 1;
    }

    int hist_size = 10;
    hist_t hist = { .actions = (color_change_t*) calloc(hist_size, sizeof(color_change_t)), .i = 0, .max_hist = hist_size };
    app.hist = &hist;
    
    if (parse_status(FILE_PROVIDED)){
        app.graph = read_graph(file_name);
        app.pgraph = graph_to_pgraph(app.graph, RANDOM_COLOR);
    }
    else if (parse_status(BUILDER_MODE)){
        app.pgraph = make_empty_pgraph(15, 100);
        app.graph = NULL;
    }

    SDL_Event e;

    while (!app.quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            handle_event(&e, &app);
        }

        render_app(gRenderer, &app);
        
    }
    // Free resources and close SDL
    free(hist.actions);
    delete_pgraph(&app.pgraph);
    delete_graph(&app.graph);
    close();
    return 0;
}

