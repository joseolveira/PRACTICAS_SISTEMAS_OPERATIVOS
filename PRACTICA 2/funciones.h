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

#define FUNCIONES_H_
#define MMALLOC  1
#define MMAP 2
#define MSHARED 3
#define ALL 4



int TrocearCadena(char * cadena, char * tokens[]);
void cmd_pid();
void IntroducirMem(void * dir,int tam,char * comand);
void IntroducirMalloc(void * dir, int tam);
void IntroducirMmap(void * dir, int tam,char * fich, int df);
void LeerMemoria(int tipo);
void EliminarMalloc(int tamano);
void EliminarMmap(char *fichero);
void EliminarShared(int tamano);
void EliminarDir(char * direccion);
void * ObtenerMemoriaShmget (key_t clave, off_t tam);
void * MmapFichero(char * fichero, int proteccion);
void cmd_recursiva(int n);
ssize_t LeerFichero(int fd, void *buf, size_t n);
ssize_t EscribirFichero(int fd, const void *buf, size_t n);
