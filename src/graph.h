#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// Contains an implementation of graph data structure as an adjacency matrix

#ifndef GRAPH_HEADER
#define GRAPH_HEADER

typedef struct graph_t {
    int size;
    int* adj_matrix;
} graph_t;

graph_t* make_graph(int size, int* arr);
void delete_graph(graph_t** g);
void print_graph(graph_t* g, FILE* stream);
int eat_whitespace(FILE* f);

typedef struct int_obj {
    int value;
    int length;
} int_obj;

extern char read_eof;
int_obj read_int(FILE* f);
graph_t* read_graph(char* file_name);

#endif
