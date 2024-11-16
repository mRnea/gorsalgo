#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "graph.h"

char read_eof = 0;

graph_t* make_graph(int size, int* arr){
    graph_t* g = (graph_t*) calloc(1, sizeof(graph_t));
    g->size = size;
    g->adj_matrix = arr;
    return g;
}

void delete_graph(graph_t** g){
    if (NULL != *g){
        free((*g)->adj_matrix);
        free(*g);
        *g = NULL;
    }
}

void print_graph(graph_t* g, FILE* stream){
    int* arr = g->adj_matrix;
    int dim = g->size;
    int start = 0;
    fprintf(stream, "%d\n", dim);
    for (int i = 0; i < dim; i++){
        start = i * dim;
        for (int j = 0; j < dim - 1; j++){
            fprintf(stream, "%d ", arr[start + j]);
        }
        fprintf(stream, "%d\n", arr[start + dim - 1]);
    }
}

int eat_whitespace(FILE* f){
    int ch;
    int i = 0;
    while (1){
        ch = fgetc(f);
        if (EOF == ch){
            read_eof = 1;
            break;
        }
        else if (!isspace(ch)){
            ungetc(ch, f);
            break;
        }
        i++;
    }
    return i;
}

// Can read non-negative integers
int_obj read_int(FILE* f){
    eat_whitespace(f);
    if (read_eof){
        return { .value = 0, .length = 0};
    }

    int i = 0;
    char buffer[16];
    int ch;
    while (1){
        ch = fgetc(f);
        if (EOF == ch){
            read_eof = 1;
            break;
        }
        else if (!isdigit(ch)){
            ungetc(ch, f);
            break;
        }
        buffer[i++] = ch;
    }
    buffer[i] = '\0';
    return { .value = atoi(buffer), .length = i};
}

graph_t* read_graph(char* file_name){
    FILE* f = fopen(file_name, "r");
    if (!f){
        fprintf(stderr, "file \"%s\" could not be opened", file_name);
        return NULL;
    }
    read_eof = 0;

    const int dim = read_int(f).value;
    int* arr = (int*) calloc(dim * dim, sizeof(int));
    int i = 0;
    int_obj num;
    while (!read_eof && i < dim * dim){
        num = read_int(f);
        if (num.length == 0){
            break;
        }
        arr[i++] = num.value;
    }
    fclose(f);
    f = NULL;
    return make_graph(dim, arr);
}

