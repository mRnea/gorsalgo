OBJS = src/color.cpp src/graph.cpp src/visualize.cpp algorithms/coloring.cpp src/main.cpp

CC = g++

# -w suppresses all warnings
# COMPILER_FLAGS = -w

LINKER_FLAGS = -lSDL2 -lSDL2_ttf

OBJ_NAME = main

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
