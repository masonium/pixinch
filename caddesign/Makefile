EXE = caddesign
OBJS = main.o

LIBS = 
INCLUDES = -I/home/mason/include

CXX_FLAGS = -std=c++0x -Wall -g $(INCLUDES) 

SFLAGS = -Wall -fsyntax-only
.PHONY: check-syntax

$(EXE): $(OBJS)
	g++ $(CXX_FLAGS) -o $@ $(OBJS) $(LIBS)

check-syntax:
	$(CXX) $(INCLUDES) $(CXX_FLAGS) $(SFLAGS) $(CHK_SOURCES)

.cpp.o:
	g++ $(CXX_FLAGS) -c -o $@ $<

clean:
	rm $(EXE) *.o
