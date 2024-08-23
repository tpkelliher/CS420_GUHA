# Change base to whatever file name you want to use for your executable.
BASE = prog42

# Generate debugging symbols.
CFLAGS += -g -Wall

# You shouldn't need to change anything below this line.

all: $(BASE)

OS := $(shell uname -s)

LIBS += -lGL -lGLU -lglut -lGLEW -lglfw -lm

CC = gcc

OBJS := $(patsubst %.c, %.o, $(wildcard *.c))

$(BASE): $(OBJS)
	$(LINK.c) -o $@ $^ $(LIBS)

clean:
	rm -f $(BASE) $(OBJS)
