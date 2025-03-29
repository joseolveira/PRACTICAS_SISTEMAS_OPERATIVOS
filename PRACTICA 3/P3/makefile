all: Practica3
Practica3: procesos.o shell.o
	   gcc -Wall -g -o shell procesos.o shell.o -lm
procesos.o: procesos.c funciones.h
		gcc -Wall -g -c procesos.c -lm
shell.o: shell.c funciones.h
	gcc -Wall -c shell.c
clean:
	rm -f *.o Practica3 *~
