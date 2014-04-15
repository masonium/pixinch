EXE = shrink
OBJS = main.o cluster.o

LIBS = -lIL -lILU `sdl2-config --libs`
INCLUDES = -I/home/mason/include

CXX_FLAGS = -Wall -g $(INCLUDES) `sdl2-config --cflags` -std=c++0x

SFLAGS = -Wall -fsyntax-only
.PHONY: check-syntax

$(EXE): $(OBJS)
	g++ $(CXX_FLAGS) -o shrink $(OBJS) $(LIBS)

check-syntax:
	$(CXX) $(INCLUDES) $(SFLAGS) $(CXX_FLAGS) $(CHK_SOURCES)

.cpp.o:
	g++ $(CXX_FLAGS) -c -o $@ $<

clean:
	rm $(EXE) *.o *~

