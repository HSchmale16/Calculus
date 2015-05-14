EXE      := app
PRES     := pres.md
CXX_FLGS := -O2 -std=c++11 -march=native -mtune=intel
LD_FLGS  := -lpthread -lSDL -lm -lgflags

all: $(EXE) $(PRES).html

$(EXE): mandelbrot.cpp
	$(info Making $(EXE))
	g++ $(CXX_FLGS) $(LD_FLGS) -o $@ $^
	strip -s $@

$(PRES).html: $(PRES)
	$(info Making Presentation)
	pandoc -s --webtex -i -t SLIDY $< -o $@ --filter pandoc-citeproc

clean:
	rm -f $(EXE)
	rm -f $(PRES).html

test: $(EXE)
	./app -orgX=0.001643721971153 -orgY=0.822467633298876

example: example.txt $(EXE)
	./app -flagfile=$<
