# Change base to whatever file name you want to use for your executable.
BASE = prog42

# Generate debugging symbols.
CXXFLAGS += -g -Wall

# You shouldn't need to change anything below this line.

all: $(BASE)

OS := $(shell uname -s)

LIBS += -lGL -lGLU -lglut -lGLEW -lglfw

CXX = g++ 

OBJS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

$(BASE): $(OBJS)
	$(LINK.cpp) -o $@ $^ $(LIBS)

clean:
	rm -f $(BASE) $(OBJS)
