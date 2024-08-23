# Makefile for building OpenGL programs.  To build foo from foo.c, run:
#
#    make foo
#
# Then, to run foo, run:
#
#     ./foo

.c:
	gcc -g -o $@ $< -lglut -lGL -lGLU -lm

clean:
	/bin/rm -f lightLab1 lightLab2
