EXE      := app
PRES     := pres.md
CXX_FLGS := -pg -O3 -std=c++11 -march=native -mtune=intel
LD_FLGS  := -lpthread -lSDL -lm -lgflags -lgmp -lmpfr

all: $(EXE) $(PRES).html

$(EXE): mandelbrot.cpp.o
	$(info Making $(EXE))
	g++ $(LD_FLGS) -o $@ $^
	strip -s $@

$(PRES).html: $(PRES)
	$(info Making Presentation)
	pandoc -s --webtex -i -t dzslides $< -o $@ \
	--filter pandoc-citeproc

clean:
	rm -f $(EXE)
	rm -rf *.o
	rm -f $(PRES).html

test: $(EXE)
	./app -orgX=0.001643721971153 -orgY=0.822467633298876

example: example.txt $(EXE)
	./app -flagfile=$<

# =====================================
# File Build Rules
# =====================================

%.cpp.o: %.cpp
	g++ -c $(CXX_FLGS) -o $@ $<
