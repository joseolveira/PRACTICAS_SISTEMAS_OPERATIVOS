#include <stdlib.h> //Declara funciones que son utiles para diferentes propositos, en especial de busqueda y ordenacion.
#include <stdio.h> //Incorporan las funciones de Entrada - Salida E/S estandar, tipos y macros
#include <unistd.h> //Libreria que contiene funciones para el manejo de directorios y archivos.
#include <sys/types.h> //Libreria que contiene funciones para el manejo de directorios y archivos.
#include <sys/stat.h> //Libreria para obtener información sobre ficheros.
#include <dirent.h> //Libreria para el manejo de directorios.
#include <string.h> //Este archivo contiene funciones para manejo de cadenas de caracteres.
#include "funciones.h"

#define MAXCADENA 2048

void ProcesarCadena(char *tokens[], int *acabar){ //*acabar apunta a la direccion de memoria qe contiene el contenido de la variable acabar
	struct stat buf; //buf es una variable a la que le asignamos la estructura stat
					 //struct contiene diversos contenidos referentes a permisos(stat)
	if ((!strcmp(tokens[0],"quit")) || (!strcmp(tokens[0],"fin")) || (!strcmp(tokens[0],"exit")))
		/*STRCMP: Compara dos cadenas de caracteres lexicograficamente. Devuelve un valor negativo si s1 < s2; 
		  0 si s1 y s2 son identicas; y un valor positivo si s1 > s2.*/
		*acabar=0;//modifica el contenido de la direcion de memoria a la que apunta el puntero
	else{
         if (!strcmp(tokens[0],"deltree")){
			if (tokens[1]!=NULL){
				lstat (tokens[1],&buf);// guarda en tokens[1] el contenido de la variable buf
				if (TipoFichero(buf.st_mode) == 'd'){ 
					deltree(tokens[1]);
				}
				else{ 
					printf("Error: No es un directorio.\n");
				}
			}
		    else
				printf("Error: Argumento vacio.\n");
		}
		else if (!strcmp(tokens[0],"list")){
			list(&tokens[1]);
		}
		else 
			printf("Wrong command!\n");
	}
}

void main(){
	int acabar=1;
	char cadena[MAXCADENA];
	char *ctroceada[MAXCADENA];
	while (acabar){
		printf("-> ");
		fgets(cadena,MAXCADENA,stdin); 
		/*FGETS: Lee una cadena s, con i caracteres del archivo f.
	      STDIN: Limpia el buffer de entrada (por teclado,...).*/
		if (TrocearCadena(cadena,ctroceada)==0)
			continue; 
			//CONTINUE: No ejecuta las instrucciones posteriores, pero no termina el bucle.
		ProcesarCadena(ctroceada, &acabar); //procesar recibe la direccion de memoria que contiene la variable acabar
	}
}
