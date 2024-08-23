# Makefile for building OpenGL programs.  To build foo from foo.c, run:
#
#    make foo
#
# Then, to run foo, run:
#
#     ./foo

checker: checker.c
	gcc -g -o checker checker.c -lglut -lGL -lGLU

textureLab: textureLab.c gltx.c
	gcc -g -o textureLab textureLab.c gltx.c -lglut -lGL -lGLU

clean:
	/bin/rm -f checker textureLab
