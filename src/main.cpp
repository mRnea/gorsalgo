#include "graph.h"
#include "visualize.h"

char* file_name;
void parse_args(int argc, char *args[]){
    char file_provided_flag = 0;
    for (int i = 0; i < argc; i++){
        if (strcmp("-load", args[i]) == 0 && i + 1 < argc){
            file_name = args[++i];
            file_provided_flag = 1;
        }
    }
    if (!file_provided_flag){
        fprintf(stderr, "file not provided\n");
        exit(1);
    }
}

int main(int argc, char *args[]) {
    parse_args(argc, args);
    srand(time(NULL));

    graph_t* graph = read_graph(file_name);

    if (!init()) {
        printf("Failed to initialize SDL2!\n");
        close();
        return 1;
    }
    
    pgraph_t* pgraph = graph_to_pgraph(graph);

    bool quit = false;

    SDL_Event e;

    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            // User presses a key
            else if (e.type == SDL_KEYDOWN) {
                // Select surfaces based on key press
                switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                default:
                    break;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN){
                maybeGrabVertex(pgraph, e.button.x, e.button.y);
                if (grabbed_vertex){
                    printf("vertex (%d, %d)\n", grabbed_vertex->x, grabbed_vertex->y);
                }
                else {
                    printf("Not in any vertex\n");
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP){
                grabbed_vertex = NULL;
            }
            else if (e.type == SDL_MOUSEMOTION){
                if (grabbed_vertex){
                    move_pvertex(grabbed_vertex, e.motion.xrel, e.motion.yrel);
                    // grabbed_vertex->x += e.motion.xrel;
                    // grabbed_vertex->y += e.motion.yrel;
                }
            }
        }
        SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0xFF );
        SDL_RenderClear(gRenderer);

        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        render_pgraph(pgraph);
        render_edges(graph, pgraph);

        SDL_RenderPresent( gRenderer );
    }
    // Free resources and close SDL
    delete_pgraph(&pgraph);
    delete_graph(&graph);
    close();
    return 0;
}


