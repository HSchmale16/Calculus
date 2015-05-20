EXE      := app
PRES     := pres.md
CXX_FLGS := -O2 -std=gnu++11 -march=native -mtune=intel
LD_FLGS  := -lpthread -lSDL -lm -lgflags -lquadmath

all: $(EXE) $(PRES).html handout.pdf

$(EXE): mandelbrot.cpp.o
	$(info Making $(EXE))
	g++ $(CXX_FLGS) $(LD_FLGS) -o $@ $^
	strip -s $@

$(PRES).html: $(PRES)
	$(info Making Presentation)
	pandoc -s --webtex -i -t dzslides $< -o $@ \
	--filter pandoc-citeproc

.PHONY: clean
clean:
	rm -f $(EXE)
	rm -rf *.o
	rm -f $(PRES).html
	rm -f *.pdf
	rm -f *.aux
	rm -f *.log
	rm -f *.out

test: $(EXE)
	./app -orgX=0.001643721971153 -orgY=0.822467633298876

example: ex1.txt $(EXE)
	./app -flagfile=$<

# =====================================
# File Build Rules
# =====================================
%.pdf: %.tex
	pdflatex -shell-escape $<

%.cpp.o: %.cpp
	g++ -c $(CXX_FLGS) -o $@ $<
