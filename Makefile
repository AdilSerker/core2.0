PLATFORM = $(shell uname)

ifeq ($(findstring Linux,$(PLATFORM)),Linux)
	LIBS = -L/usr/local/lib -lglfw -lGLEW -lGL
endif

ifeq ($(findstring Darwin,$(PLATFORM)),Darwin)
	LIBS = -L/usr/local/lib -lglfw -lGLEW -framework OpenGL
endif
 
FLAGS = -O3 -ffast-math -Wall

SRC = source/*.cpp source/**/*.cpp

INC = -I headers

CC = g++ -std=gnu++11

DBG = -g1 -rdynamic

all:
	$(CC) $(FLAGS) $(SRC) $(INC) -c && make build

dbg:
	$(CC) $(DBG) $(SRC) $(INC) -c && make build_debug

scene: source/modules/Scene.cpp
	$(CC) $(FLAGS) $(INC) $< -c && make build

camera: source/modules/Camera.cpp
	$(CC) $(FLAGS) $(INC) $< -c && make build

ter: source/geometry/Terrain.cpp
	$(CC) $(FLAGS) $(INC) $< -c && make build

build:
	$(CC) ./*.o $(LIBS) $(INC) -o a.out
build_debug:
	$(CC) ./*.o $(LIBS) $(INC) -o debug


clean:
	rm -rf ./*.o
