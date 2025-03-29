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

int TrocearCadena(char * cadena, char * tokens[]){
	int i=1;
	if ((tokens[0]=strtok(cadena," \n\t"))==NULL){
		return 0;
	}
	while ((tokens[i]=strtok(NULL," \n\t"))!=NULL){
		i++;
	}
	return i;
}
void cmd_pid() {

		printf ("Identificador del proceso padre: %d\n",getppid());
		printf ("Identificador del proceso: %d\n",getpid());
}

//PRÁCTICA 3: PROCESOS.


void cmd_getPrio(char *arg){
	
	int prioridad;
	pid_t pid;
	
	if(arg == NULL)
		pid = getpid();
	else 
		pid = atoi(arg);
	if((prioridad = getpriority(PRIO_PROCESS,pid))==-1)
		perror("Error obteniendo la prioridad del proceso");
	else 
		printf("La prioridad del proceso con PID %d es %d\n", pid, prioridad);
	
}

void cmd_setPrio(char *arg[]){
	
	int prioridad;
	pid_t pid;
	
	if(arg[0]==NULL)
		cmd_getPrio(NULL);
	else if (arg[1]==NULL){
		cmd_getPrio(arg[0]);
	}
	else{
		pid = atoi(arg[0]);
		prioridad = atoi(arg[1]);
		if((setpriority(PRIO_PROCESS,pid,prioridad))==-1)
			perror("Error cambiando la prioridad del proceso");
		else 
			printf("La nueva prioridad del proceso con PID %d es %d\n",pid,prioridad);
	}
}

void cmd_setpriority(char *arg[]){
	
	if((arg[0]!=NULL) && (arg[1]==NULL))
		cmd_getPrio(arg[0]);
	else if ((arg[0]!=NULL) && (arg[1]!=NULL)){
		cmd_setPrio(&arg[0]);
	}
	else 
		printf("No ha introducido los datos.");
	
}


void cmd_fork(){
	
	int pid;
	pid=fork();
	if(pid==-1) 
		perror("Error creando el proceso hijo.");
	waitpid(pid,NULL,0); //WAITPID():waitpid(pid_t pid, int *status, int options);
						 //suspende la ejecución del proceso de llamada hasta que un 
						 //hijo especificado por el argumento pid ha cambiado de estado.
						 //De forma predeterminada, waitpid() solo espera a los hijos 
						 //terminados.
}


int comprobarPri(char *arg[]){
	
	int i=0,correcto=0,valor=-40;
	char *aux[MAXCADENA];

	while(arg[i]!=NULL){

		if (arg[i][0] =='@'){
			correcto=1;
			aux[0]=strtok(arg[i],"@\n\t");
			valor=atoi(aux[0]);
			break;
		}
		i++;
	}
	if(correcto)
		return valor;
	return valor;		
}

int comprobarEt(char *arg[]){

	int i=0,correcto=0;
	
	while(arg[i]!=NULL){
		if(arg[i][0]=='&')
			correcto=1;
		i++;
	}
	if(correcto)
		return correcto;
	else
		return correcto;
}

char *borrarPri(char *arg[]){

	int i=0;
	
	while(arg[i]!=NULL){
		if(arg[i][0]=='@')
			arg[i]='\0';
		i++;
	}
	return *arg;
}

char *borrarEt(char *arg[]){

	int i=0;
	
	while(arg[i]!=NULL){
		if(arg[i][0]=='&')
			arg[i]='\0';
		i++;
	}
	return *arg;
}

void cmd_exec(char *arg[]){
	
	execvp(arg[0],arg);
		//execvp(const char *file, char *const argv[]);
		//EXECVP(): proporciona un array de punteros a strings de terminacion nula
		//que representa la lista de argumentos disponibles para el nuevo programa.
		//el primer argumento, por convencion, debe apuntar al nombre de archivo 
		//asociado que está ejecutando.
	perror("Error ejecutando el programa.\n");	
	
}
void cmd_exec_pri(char *arg[]){
	
	int prioridad;
	pid_t pid;
	
	pid=getpid();
	prioridad = comprobarPri(arg);
	if(prioridad>=-20 && prioridad<=19){
		if(setpriority(PRIO_PROCESS,pid,prioridad)!=-1){
			*arg=borrarEt(&arg[0]);
			*arg=borrarPri(&arg[0]);
			cmd_exec(&arg[0]);
		}
		else
			printf("Error al seleccionar la prioridad.\n");
	}
	else if(prioridad==-40){
		*arg=borrarEt(&arg[0]);
		cmd_exec(&arg[0]);
	}
	else 
		printf("La prioridad no es correcta.\n");
}


void cmd_prog(char *arg[], struct nodo **lista){

	pid_t pid;
	time_t tiempo;
	
	pid=fork();
	switch(pid){
		case -1: perror("Error creando el proceso hijo\n");break;
		case 0: cmd_exec_pri(arg); break;
		default:
				if(comprobarEt(arg)){
					time(&tiempo);
					insertarProceso(pid,arg,tiempo,lista);
				}
				else
					waitpid(pid,NULL,0);
				break;
	}
}

static struct nodo *crearNodo(struct nodo **p){
	
	*p=malloc(sizeof(struct nodo));
	if(*p==NULL)
		perror("Imposible crear el nuevo nodo\n");
	else
		(*p)->sig=NULL;
	return *p;
}

void insertarProceso(pid_t pid, char *arg[], time_t tiempo, struct nodo **lista){
	int i=0;
	struct nodo *nuevo; 
	
	crearNodo(&nuevo);
	
	nuevo->procesos.estado= -1;
	nuevo->procesos.pid=pid;
	nuevo->procesos.prioridad=getpriority(PRIO_PROCESS,pid);
	nuevo->procesos.inicio=tiempo;
	while (arg[i]!=NULL){
		strcat(nuevo->procesos.argumentos,arg[i]);
		strcat(nuevo->procesos.argumentos," ");
		i++;
	}
	if(*lista==NULL)
		*lista=nuevo;
	else{
		nuevo->sig=*lista;
		*lista=nuevo;
	}
}

void cmd_jobs(struct nodo **lista){
	
	struct nodo *aux=*lista;
	
	if(aux==NULL)
		printf("La tabla de procesos esta vacia.\n");
	while(aux!=NULL){
		actualizarEstados(aux);
		mostrarProceso(aux);
		aux=aux->sig;
	}
}

void actualizarEstados(struct nodo *aux){
	
	int estado;
	
	while(aux!=NULL){
		waitpid((pid_t)aux->procesos.pid,&aux->procesos.estado, WNOHANG | WUNTRACED | WCONTINUED);
		if(WIFSTOPPED(aux->procesos.estado)){
			estado=WSTOPSIG(aux->procesos.estado);
		aux->procesos.estado=estado;
		}
		else if(WIFEXITED(aux->procesos.estado)){
			estado=WEXITSTATUS(aux->procesos.estado);
			aux->procesos.estado=estado;	
		}
		else if(WIFSIGNALED(aux->procesos.estado)){
			estado=WTERMSIG(aux->procesos.estado);
			aux->procesos.estado=estado;
		}
		aux=aux->sig;
	}
}

void mostrarProceso(struct nodo *aux){
	
	pid_t pid;
	time_t t;
	int prioridad;
	int estado;
	char argumentos[MAXCADENA];
	char buf[26];
	struct tm *tm;
	
	pid=aux->procesos.pid;
	prioridad=getpriority(PRIO_PROCESS,pid);
	t=aux->procesos.inicio;
	estado=aux->procesos.estado;
	sprintf(argumentos,"%s",aux->procesos.argumentos);
	tm=localtime(&t);
	strftime(buf, sizeof(buf),"%H:%M:%S", tm);
	
	printf("%4d%5d%13s",pid,prioridad,buf);
	waitpid(pid,&estado, WNOHANG | WUNTRACED | WCONTINUED);
	if (WIFSTOPPED(estado)){
		printf("%15s %d ","PARADO",estado);	
		printf("%27s\n",argumentos);
	}
	else if (WIFEXITED(estado)){ 
		printf("%24s %d","TERMINADO NORMALMENTE",estado);
		printf("%22s\n",argumentos);	
	}
	else if (WIFSIGNALED(estado)){
		printf("%24s %d ","TERMINADO POR SEÑAL",estado);
		printf("%22s\n",argumentos);
	}
	else if ((WIFCONTINUED(estado)) || (estado == -1)){
		printf("%18s ","ACTIVO");
		printf("%29s\n",argumentos);		
	}
}


void cmd_proc(char *arg, struct nodo **lista){
	
	struct nodo *aux;
	int correcto=0, pid;
	
	if(arg==NULL)
		cmd_jobs(lista);
	else{
		aux=*lista;
		pid=atoi(arg);
		while(aux!=NULL){
			if(aux->procesos.pid==pid){
				correcto=1;
				break;
			}
			aux=aux->sig;
		}
		if(correcto)
			mostrarProceso(aux);
		else
			cmd_jobs(lista);
	}
}

void cmd_clearjobs(char *arg, struct nodo **lista){
	
	struct nodo *aux=*lista;
	
	if(arg==NULL){
		printf("Comando inadecuado.\n");
		return;
	}
	if(!WIFEXITED(aux->procesos.estado) && !WIFSIGNALED(aux->procesos.estado))
		waitpid(aux->procesos.pid,NULL,0);
	else{
		while(aux!=NULL){
			if(!strcmp(arg,"-term") && WIFEXITED(aux->procesos.estado))
				eliminarEntradaProceso(aux,lista);
			else if(!strcmp(arg,"-sig") && WIFSIGNALED(aux->procesos.estado))
				eliminarEntradaProceso(aux,lista);
			aux=aux->sig;
		}
	}
}

void eliminarEntradaProceso(struct nodo *aux, struct nodo **lista){
	
	struct nodo *tmp=*lista;
	
	if(tmp==aux)
		tmp=aux->sig;
	else{
		while(tmp->sig!=aux){
			tmp=tmp->sig;
		}
		tmp->sig=aux->sig;
	}
	*lista=tmp;
}
