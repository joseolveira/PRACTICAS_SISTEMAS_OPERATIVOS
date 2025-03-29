//Juan Mosquera Cid (juan.mosquera.cid)
//Olalla Rama García (olalla.rama.garcia)

#include <sys/types.h>
#include <sys/stat.h>
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

#define MAXCADENA 2048

struct elementos{
	pid_t pid;
	int prioridad;
	char argumentos[MAXCADENA];
	time_t inicio;
	int estado;
};
struct nodo{
	struct elementos procesos;
	struct nodo *sig;
};



int TrocearCadena(char * cadena, char * tokens[]);
void cmd_pid();
//PRÁCTICA3:PROCESOS
void cmd_getPrio(char *arg);
void cmd_setPrio(char *arg[]);
void cmd_setpriority(char *arg[]);
void cmd_fork();
int comprobarPri(char *arg[]);
int comprobarEt(char *arg[]);
char *borrarPri(char *arg[]);
char *borrarEt(char *arg[]);
void cmd_exec(char *arg[]);
void cmd_exec_pri(char *arg[]);
void cmd_prog(char *arg[], struct nodo**lista);
void insertarProceso(pid_t pid, char *arg[], time_t tiempo, struct nodo **lista);
void cmd_jobs(struct nodo **lista);
void actualizarEstados(struct nodo *aux);
void mostrarProceso(struct nodo *aux);
void cmd_proc(char *arg, struct nodo **lista);
void cmd_clearjobs(char *arg, struct nodo **lista);
void eliminarEntradaProceso(struct nodo *aux, struct nodo **lista);
