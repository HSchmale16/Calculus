EXE  := mandelbrot

all: $(EXE)

$(EXE): mandelbrot.cpp
	g++ -std=c++11 -O2 -lpthread -lSDL -lm -o $@ $^
	strip -s $@
