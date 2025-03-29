all: shell
shell: funciones.o misfunciones.o
	   gcc -g -o shell funciones.o misfunciones.o -lm
funciones.o: funciones.c funciones.h
		gcc -g -c funciones.c -lm
misfunciones.o: misfunciones.c funciones.h
		gcc -c misfunciones.c 
