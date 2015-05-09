EXE  := app
PRES := pres.md

all: $(EXE) $(PRES).html

$(EXE): mandelbrot.cpp
	$(info Making $(EXE))
	g++ -O2 -std=c++11 -lpthread -lSDL -lm -lgflags -o $@ $^
	strip -s $@

$(PRES).html: $(PRES)
	$(info Making Presentation)
	pandoc -s -i -t SLIDY $< -o $@ --filter pandoc-citeproc

clean:
	rm -f $(EXE)
	rm -f $(PRES).html
