PLATFORM = $(shell uname)

ifeq ($(findstring Linux,$(PLATFORM)),Linux)
	LIBS = -L/usr/local/lib -lglfw -lGLEW -lGL
endif

ifeq ($(findstring Darwin,$(PLATFORM)),Darwin)
	LIBS = -L/usr/local/lib -lglfw -lGLEW -framework OpenGL
endif
 
FLAGS = -O3 -Wall #-ffast-math

SRC = source/*.cpp source/**/*.cpp

INC = -I headers

CC = g++ -std=gnu++11

DBG = -g

all:
	$(CC) $(FLAGS) $(SRC) $(INC) -c && make build

dbg:
	$(CC) $(DBG) $(SRC) $(INC) -c && make build_debug

scene: source/modules/Scene.cpp
	$(CC) $(FLAGS) $< $(INC) -c && make build

camera: source/modules/CameraOrbit.cpp
	$(CC) $(FLAGS) $< $(INC) -c && make build

app: source/core/App.cpp
	$(CC) $(FLAGS) $< $(INC) -c && make build

ter: source/geometry/Terrain.cpp
	$(CC) $(FLAGS) $< $(INC) -c && make build

char: source/character/Character.cpp
	$(CC) $(FLAGS) $< $(INC) -c && make build

build:
	$(CC) ./*.o $(LIBS) -o a.out
build_debug:
	$(CC) ./*.o $(LIBS) -o debug.out


clean:
	rm -rf ./*.o

clean_pre:
	rm -rf ./headers/*.gch
