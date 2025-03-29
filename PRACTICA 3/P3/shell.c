//Juan Mosquera Cid (juan.mosquera.cid)
//Olalla Rama García (olalla.rama.garcia)

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include "funciones.h"

#define MAXCADENA 2048

void ProcesarCadena(char *tokens[], int *acabar, struct nodo **lista){
	if ((!strcmp(tokens[0],"end")) || (!strcmp(tokens[0],"fin")) || (!strcmp(tokens[0],"exit")))
		*acabar=0;
	else{
		if (!strcmp(tokens[0],"pid"))
			cmd_pid();
		if (!strcmp(tokens[0],"setpriority"))
			cmd_setpriority(&tokens[1]);
		else if(!strcmp(tokens[0],"fork"))
			cmd_fork();
		else if (!strcmp(tokens[0],"exec"))
			cmd_exec_pri(&tokens[1]);
		else if(!strcmp(tokens[0],"jobs"))
			cmd_jobs(lista);
		else if(!strcmp(tokens[0],"proc"))
			cmd_proc(tokens[1],lista);
		else if(!strcmp(tokens[0],"clearjobs"))
			cmd_clearjobs(tokens[1],lista);
		else 
			cmd_prog(tokens,lista);
	}
}

int main(){

	int acabar=1;
	char cadena[MAXCADENA];
	char *ctroceada[MAXCADENA];
	struct nodo *lista;

	while (acabar){
		printf("$");
		fgets(cadena,MAXCADENA,stdin); 

		if (TrocearCadena(cadena,ctroceada)==0)
			continue; 

		ProcesarCadena(ctroceada, &acabar, &lista);
	}
}

