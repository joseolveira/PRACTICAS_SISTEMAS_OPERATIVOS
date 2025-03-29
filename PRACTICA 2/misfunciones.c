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
#include "funciones.h"



struct elementosMem{
	void * puntero;
	int descriptorF;
	int tamano;
	int tipo;
	char instiempo[30];
	char direccion[30];
	char comando[30];
	char fichero[255];
	key_t clave;
	struct elementosMem *sig;
	};


struct elementosMem *inicio = NULL;

void IntroducirMem(void * dir,int tam,char * command){
	
	struct elementosMem *aux = inicio;
	struct elementosMem *mem = (struct elementosMem*) malloc(sizeof(struct elementosMem));
	time_t fecha;
	char tiempo[30];
	int i= 0;
		time(&fecha);
		mem->puntero = dir;
		sprintf(mem->direccion,"%p", dir) ;
		mem->tamano = tam ;
		sprintf(mem->instiempo,"%s",ctime(&fecha));
		sprintf(mem->comando,"%s",command);		
		mem->sig = NULL;
		if(inicio==NULL)
			inicio = mem;
		else{
			while(aux->sig!=NULL)
				aux=aux->sig;
			aux->sig = mem;
		}
}
void IntroducirMalloc(void * dir, int tam){
	
	struct elementosMem *aux = inicio; 
	struct elementosMem *mem = (struct elementosMem*) malloc(sizeof(struct elementosMem)); 
	time_t fecha;
	char tiempo[30];
		time(&fecha);
		mem->puntero = dir; 
		sprintf(mem->direccion,"%p", dir); //SPRINF():escribe la salida bajo el control de un formato string que especifica 
										   //como los argumentos a los que se accede.
		mem->tamano = tam ;
		sprintf(mem->instiempo,"%s",ctime(&fecha)); //CTIME():Devuelve una cadena que representa la hora local basada en el argumento(fecha que es de tipo t_time).
		sprintf(mem->comando,"%s","malloc");
		mem->tipo = MMALLOC;		
		mem->sig = NULL; 
		if(inicio==NULL) 
			inicio = mem; 
		else{
			while(aux->sig!=NULL) 
				aux=aux->sig;
			aux->sig = mem; 
		}
}
void IntroducirMmap(void * dir, int tam,char * fich, int df){
	
	struct elementosMem *aux = inicio;
	struct elementosMem *mem = (struct elementosMem*) malloc(sizeof(struct elementosMem));
	time_t fecha;
	char tiempo[30];
		time(&fecha);
		mem->puntero = dir;
		sprintf(mem->direccion,"%p", dir) ;
		mem->tamano = tam ;
		sprintf(mem->instiempo,"%s",ctime(&fecha));
		sprintf(mem->comando,"%s","mmap ");
		sprintf(mem->fichero,"%s",fich);
		mem->tipo = MMAP;
		mem->descriptorF = df;		
		mem->sig = NULL;
		if(inicio==NULL)
			inicio = mem;
		else{
			while(aux->sig!=NULL)
				aux=aux->sig;
			aux->sig = mem;
		}
}
void IntroducirShared(void * dir, int tam, key_t key){
	
	struct elementosMem *aux = inicio;
	struct elementosMem *mem = (struct elementosMem*) malloc(sizeof(struct elementosMem));
	time_t fecha;
	char tiempo[30];
		time(&fecha);
		mem->puntero = dir;
		sprintf(mem->direccion,"%p", dir) ;
		mem->tamano = tam ;
		sprintf(mem->instiempo,"%s",ctime(&fecha));
		sprintf(mem->comando,"%s","shm ");
		mem->clave = key;
		mem->tipo = MSHARED;		
		mem->sig = NULL;
		if(inicio==NULL)
			inicio = mem;
		else{
			while(aux->sig!=NULL)
				aux=aux->sig;
			aux->sig = mem;
		}
}
/*Recibe un int (mmalloc|mmap|mshared|all) y muestra la lista*/
void LeerMemoria(int tipo){
	
	struct elementosMem *aux = inicio;
	char *direccion;
	int tamano;
	char *instiempo;
	char *comando;
	char *fichero;
	int descriptorF;
	key_t clave;
	int tipoM;
	while (aux!=NULL){
		direccion=aux->direccion;
		tamano=aux->tamano;
		instiempo=aux->instiempo;
		comando=aux->comando;
		fichero=aux->fichero;
		descriptorF=aux->descriptorF;
		clave=aux->clave;
		tipoM=aux->tipo;
		aux=aux->sig;
		if((tipo==ALL||tipo==MMALLOC)&&tipoM==MMALLOC) //tipoM contiene el tipo de la estructura. Si aun no metimos nada en la estructura tipoM va a ser NULL
													   //por lo que ya no va a entrar ahi.
			printf("%14s size: %10d. %s %s",direccion,tamano,comando,instiempo);
		if((tipo==ALL||tipo==MSHARED)&&tipoM==MSHARED)
			printf("%14s size: %10d. %s (key %d) %s",direccion,tamano,comando,clave,instiempo);
		if((tipo==ALL||tipo==MMAP)&&tipoM==MMAP)
			printf("%14s size: %10d. %s %s (fd:%d) %s",direccion,tamano,comando,fichero,descriptorF,instiempo);
	}
}

/*Desasigna en función del tamaño*/
void EliminarMalloc(int tamano) { 
	struct elementosMem *aux = inicio;
	struct elementosMem *tem = NULL; 
	void * p;
	while (aux!=NULL){ 
		if(aux->tipo == MMALLOC && aux->tamano == tamano){ 
			p=aux->puntero; 
			tem = aux;  
			if(aux==inicio){ 
				inicio=inicio->sig; 
				aux = inicio; 
			}

			else{
				while(aux->sig!=tem)
					aux=aux->sig; 
				aux->sig=tem->sig;
			}
			free(p); 
			free(tem); //se libera la estructura que antes contenia tem
			break; 
		}else
			aux=aux->sig;
		
	}
}
/*Desasigna en función del fichero*/
void EliminarMmap(char *fichero){
	
	struct elementosMem *aux = inicio;
	struct elementosMem *tem = NULL;
	void * p;
	while (aux!=NULL){
		if(aux->tipo == MMAP && !strcmp(aux->fichero,fichero)){
			p=aux->puntero;
			tem = aux;
			if(aux==inicio){
				inicio=inicio->sig;
				aux = inicio;
			}
			else{
				while(aux->sig!=tem)
					aux=aux->sig;
				aux->sig=tem->sig;
			}
			munmap(p,tem->tamano);
			//MUNMAP(): elimina cualquier asignacion creada anteriormente
			//con el comando MAP(). Se le pasa la direccion y la longitud.
			free(tem);
			break;
		}
		else
			aux=aux->sig;
		
	}
}
/*Desasigna memoria compartida en función de su tamaño*/
void EliminarShared(int tamano){
	
	struct elementosMem *aux = inicio;
	struct elementosMem *tem = NULL;
	void * p;
	while (aux!=NULL){
		if(aux->tipo == MSHARED && aux->tamano == tamano){
			p=aux->puntero;
			tem = aux;
			if(aux==inicio){
				inicio=inicio->sig;
				aux = inicio;
			}
			else{
				while(aux->sig!=tem)
					aux=aux->sig;
				aux->sig=tem->sig;
			}
			shmdt(p);
			//SHMDT(): separa el segmento de memoria compartida ubicado
			//en la direccion especificada(p) desde el espacio de direcciones
			//del proceso llamante (libera la direccion de memoria compartida)
			free(tem);
			break;
		}else
			aux=aux->sig;
		
	}
}

/*Desasigna en función de la dirección 0x7ff...*/
void EliminarDir(char * direccion){
	
	struct elementosMem *aux = inicio;
	struct elementosMem *tem = NULL;
	void * p;
	int i=0;
	while (aux!=NULL){
		if(!strcmp(aux->direccion,direccion)){
			p=aux->puntero;
			tem = aux;
			if(aux==inicio){
				inicio=inicio->sig;
				aux = inicio;
			}
			else{
				while(aux->sig!=tem)
					aux=aux->sig;
				aux->sig=tem->sig;
			}
			if(tem->tipo==MMALLOC){ 
				free(p);
				i=1;
			}				
			else if (tem->tipo==MMAP){
					munmap(p,tem->tamano);
					i=1;
			}
				else if (tem->tipo==MSHARED){
						shmdt(p);
						i=1;
				}
			free(tem);
			break;
		}
		else
			aux=aux->sig;	
	}
	if(i==0)
		LeerMemoria(ALL);
}

void * ObtenerMemoriaShmget (key_t clave, off_t tam){ //off_t tam : tamaño en bytes del fichero
	
	void * p;
	int aux,id,flags=0777; //flags = 0777 Privilegios a todos (root por default) (0 por ser octal)
	struct shmid_ds s; // SHMID_DS: contiene informacion sobre el segmento
	if (tam)
		flags=flags | IPC_CREAT | IPC_EXCL; // Para crear segmento memoria y devolver error si ya existe
	if (clave==IPC_PRIVATE)   /*no nos vale*/{
		errno=EINVAL; 
		return NULL;
	}
	if ((id=shmget(clave, tam, flags))==-1) //shmget devuelve el id del espacio compartido
		return (NULL);
		

	if ((p=shmat(id,NULL,0))==(void*) -1){ //(void*) -1 si hay error
		//asigna a p la direccion de memoria para ese segmento concreto
		aux=errno;
		if (tam)
			shmctl(id,IPC_RMID,NULL);// IPC_RMID: marca el segmento que va a destruirse
			//SHMCTL(): realiza la operación de control especificada por 
			//cmd en el segmento de memoria compartida para el id dado
		errno=aux;
		return (NULL);
	}
	shmctl (id,IPC_STAT,&s);// Guarda la información en s, struct shmid_ds
	//IPC_STAT copia la informacion para ese id en s para luego poder usar todo lo referente a ese segmento de memoria
	IntroducirShared(p,s.shm_segsz, clave);// s.shm_segsz = tamaño en bytes
	return (p);
}

void * MmapFichero (char * fichero, int proteccion){
	
	int df,map=MAP_PRIVATE,modo=O_RDONLY;
	//MAP_PRIVATE: Cree una asignación de copia-en-escritura privada.
	struct stat s; 
	void * p;


	if (proteccion&PROT_WRITE) modo=O_RDWR; //O_RDWR: abierto para leer y escribir
	//PROT_WRITE: Las paginas pueden escribirse

	if (stat(fichero,&s)==-1 || (df=open(fichero,modo))==-1) 
		return NULL;
	
	if ((p=mmap (NULL,s.st_size, proteccion,map,df,0))==MAP_FAILED) 
	//MAP_FAILED: en caso de exito,mmap() devuelve un puntero al area asignada. En caso de error,
	//se devuelve el valor MAP_FAILED(es decir,(void*)-1) y errno para indicar la causa del error.
		return NULL;

	IntroducirMmap(p,s.st_size,fichero,df);
	return p;
}

ssize_t LeerFichero(int fd, void *buf, size_t n){
		
	ssize_t ret;
	ret = read(fd, buf,n); //read:lee sobre un descriptor de fichero.Esta incluido en la libreria #include <unistd.h>
	return ret;
}

ssize_t EscribirFichero(int fd, const void *buf, size_t n){
	
	ssize_t ret;
	ret = write(fd, buf,n); //write: envia un mensaje a otro usuario
	return ret;
}

