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


#define MAXCADENA 2048


int a = 150;
float b = 320;
double c = 120;


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
/**Asigna tam bytes mediante malloc e informa de la dirección donde se ha asignado**/
void cmd_malloc(char * arg[]){
	
	int size=-1;
	void *p;
	if(arg[1]==NULL){
		LeerMemoria(MMALLOC);
	}
	else{	
		sscanf(arg[1],"%d",&size);
		//SSCANF(): obtiene la entrada que se va a analizar de una cadena de caracteres dada (string)
		if(size>-1){
			p = (void*) malloc(size); //se le asigna a p la direccion de memoria para un tamaño
			printf("Allocated %d at %p \n",size,p);
				IntroducirMalloc(p,size);
		}
	}
}

void cmd_mmap (char * arg[]){
//mmap(): crea una nueva asignacion en el espacio de direcciones virtuales
//de la llamada proceso.
	
	char * perm;
	void * p;
	int proteccion = 0;
	
	if (arg[0]==NULL){ 
		LeerMemoria(MMAP);
		return;
	}
	if ((perm=arg[1])==NULL){
		printf("Comando erroneo. No ha introducido los permisos.\n");
		return;
	}
	else if ((perm=arg[1])!=NULL && strlen(perm)<4){
			if (strchr(perm,'r')!=NULL) proteccion|=PROT_READ; 
		//STRCHR(): Busca cierta letra dentro de un string. Null si no la contiene o un puntero a la posicion en que
		//se encuentra. En caso contrario se le pasa la cadena a analizar y la letra que se busca
			if (strchr(perm,'w')!=NULL) proteccion|=PROT_WRITE; 
															    
			if (strchr(perm,'x')!=NULL) proteccion|=PROT_EXEC;  
			
	}
	else{
		printf("No está introduciendo bien los permisos.\n");
		return;
	}
	

	if ((p=MmapFichero(arg[0],proteccion))==NULL)
		perror("Imposible mapear fichero");
	else
		printf("file %s mapped at %p \n", arg[0], p);
}

void cmd_shared (char *arg[]){
	
	key_t k; 
	off_t tam=0; 
	void *p;
	int size=-1;
	
	if(arg[0] == NULL){
		LeerMemoria(MSHARED);
		return;
	}
	
	k=(key_t) atoi(arg[0]); //atoi: convierte unn string en un entero
	if (arg[1]!=NULL)
		tam=(off_t) atoi (arg[1]);
	if ((p=ObtenerMemoriaShmget(k,tam))==NULL)
		perror("Cannot allocate");
	else
		printf("Allocated shared memory %d at %p\n",k,p);
}

void assign (){
	LeerMemoria(ALL);
}

void cmd_assign(char *arg[]){
	
	if(arg[0]==NULL){
		assign();
	}
	
	else if(!strcmp(arg[0],"-malloc")){
		cmd_malloc(&arg[0]);
	}
	else if (!strcmp(arg[0],"-mmap")){
		cmd_mmap(&arg[1]);
	}
	else if(!strcmp(arg[0],"-shared")){
		cmd_shared(&arg[1]);
	}

}

void cmd_deassign (char * arg[]){
	
	int size=-1;

	if(arg[0]==NULL){
		printf("Error: falta el comando\n");
		return;
	}
	
	else if(!strcmp(arg[0],"-malloc")){
		if(arg[1]==NULL)
			LeerMemoria(MMALLOC);
		else	{
			sscanf(arg[1],"%d",&size);
			if(size>-1)
				EliminarMalloc(size);
			}
	}
	else if(!strcmp(arg[0],"-mmap")){
		if (arg[1] == NULL)
			LeerMemoria(MMAP);
		else
		{
			EliminarMmap(arg[1]);
			return;
		}
	}
	else if(!strcmp(arg[0],"-shared")){
		if(arg[1]==NULL)
			LeerMemoria(MSHARED);
		else{
			sscanf(arg[1],"%d",&size);
			if(size>-1)
				EliminarShared(size);
		}
	}
	else 
		EliminarDir(arg[0]);
}


void cmd_rmkey (char * key){
	
	key_t clave; //key_t: tipo de la clave
	int id;
	
	if (key == NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){ 
		//STRTOUL:convierte una cadena a un long-integer
		//cuando el valor clave es igual a IPC_PRIVATE es que la clave es valida y por tanto se puede crear. 
		printf("   rmkey  clave_valida\n");
		return;
	}
	if ((id=shmget(clave,0,0666))==-1){ //no se puede acceder al espacio de memoria compartido
		//SHMGET():crea la memoria y nos devuelve un identificador para dicha zona.
		//Si la zona de memoria correspondiente a la Clave key_t ya estuviera creada, simplemente nos
		//daría el id de la memoria

		perror ("shmget: imposible obtener memoria compartida");
		return;
	}
	if (shmctl(id,IPC_RMID, NULL)==-1) //el id es el identificador del espacio de memoria compartida
		//shmclt(key_t key, size_t size, int shmflg)
		perror ("shmctl: imposible eliminar memoria compartida\n");

}

void cmd_mem(){ 
	
	int x = 2;
	float y = 2.3;
	double z = 5;

	void (*f1) = cmd_malloc;;
	void (*f2) = cmd_mmap;
	void (*f3) = cmd_shared;


	printf("VAR GLOB de tipo float: %p\n",&b);
	printf("VAR GLOB de tipo double: %p\n",&z);
	printf("VAR GLOB L_shared: %p\n",&c);
	printf("VAR LOC de tipo int: %p\n",&x);
	printf("VAR LOC de tipo int: %p\n",&a);
	printf("VAR LOC de tipo float: %p\n",&y);
	printf("FUN cmd_mmalloc(): %p\n",f1);
	printf("FUN cmd_mmap(): %p\n",f2);
	printf("FUN cmd_mshared(): %p\n",f3);

}

char procesarCaracteres(char c){

	if (isascii(c) && (isprint(c)))
		return c;
	else return ' ';
}


void cmd_memdump(char *arg[]){
	char *p;
	int i=0, j=0, cont = 25, acu = 25;
	if (arg[0]==NULL){ 
		printf("Error: faltan los argumentos.\n");
		return;
	}
	if (arg[1]!=NULL)
		cont = atoi(arg[1]); 
	p =(char *) strtoull(arg[0], NULL, 16); //STRTOULL(): devuelve un valor de tipo int largo(long long)
											//se le pasa el arg[0], un valor null y la base(hexadecimal(16))
											//en p guarda los tamaños de cada caracter que son de tio char.
											
	if (cont<acu)
		acu=cont;
	while (j<cont){
		for (i=j;i<acu+j;i++)
			printf("  %c",procesarCaracteres(p[i]));
		printf("\n");
		for (i=j;i<acu+j;i++)
			printf(" %x",(unsigned char) p[i]);
		printf("\n");
		j=j+25;
		if (cont-j<25)
			acu=cont-j;
	}
}


void cmd_recursiva (int n){

	char automatico[MAXCADENA];
	static char estatico[MAXCADENA];

	printf ("Parametro n:%d en %p\n",n,&n);
	printf ("Array estatico en:%p \n",estatico);
	printf ("Array automatico en %p\n",automatico);
	if (n>0)
		cmd_recursiva(n-1);
}

void Recursiva(char *arg){
	if (arg==NULL)
		printf("Error: falta el argumento\n");
	else
		cmd_recursiva(atoi(arg));
}

void cmd_read (char * arg[]){
	int file,n;	
	void * p;	
	size_t cont = -1; 
	ssize_t leido;	//ssize_t:se utiliza para funciones cuyo valor 
					//devuelto podría ser un tamaño válido o un valor
					//negativo para indicar un error.
	struct stat st;	

	file = open(arg[0],O_RDONLY);

	if(file<0) 
		perror("Error abriendo");
	sscanf(arg[1],"%p",&p);
	if(arg[2]==NULL){ //si no se le pasa argumento coje el tamaño total del archivo
		fstat(file,&st); //obtiene toda la informacion del fichero abierto
		cont=st.st_size; 
	}
	else{
		sscanf(arg[2],"%u",&n); //%u: unsigned integer
		cont = (size_t) n; //se le asigna a cont ese tamaño
	}	
	leido=LeerFichero(file, p, cont);		
	if(leido<0)
		perror("Error de lectura");	
	else	
		printf("Leidos %zu bytes en %p\n",leido,p);					
	close(file);	
}

void cmd_write (char * arg[]){
	int file,n;
	void *p;
	size_t cont = -1;
	ssize_t escrito;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; 

	sscanf(arg[1],"%p",&p); //obtenemos la direccion de memoria
	sscanf(arg[2],"%u",&n); //y el tamaño
		cont = (size_t) n;
	if(arg[3]!=NULL && !strcmp(arg[3],"-o"))	
		file = open(arg[0],O_WRONLY | O_CREAT | O_TRUNC,mode);
		//O_CREAT:Si el archivo existe este identificador no tiene efecto;de lo contrario se crea el archivo. 
		//O_TRUNC:Si el archivo existe y se abre con exito O_RDWR u O_RDONLY, su longitud se trunca a 0 
		//y el modo y el propietario no cambian
	else
		file = open(arg[0],O_WRONLY | O_CREAT,mode);
	if(file<0) 
		perror("Error creando");
	escrito = EscribirFichero(file, p, cont);
	if(escrito<0)
		perror("Error escribiendo");
	else
		printf("Escritos %zu de %p en %s\n",escrito,p,arg[0]);
	close(file);
}
void cmd_iofile(char *arg[]){

	if(!strcmp(arg[0],"-read")){
		cmd_read(&arg[1]);
	}
	else if(!strcmp(arg[0],"-write")){
		cmd_write(&arg[1]);
	}
}

void ProcesarCadena(char *tokens[], int *acabar){
	struct stat buf;
	if ((!strcmp(tokens[0],"end")) || (!strcmp(tokens[0],"fin")) || (!strcmp(tokens[0],"exit")))
		*acabar=0;
	else{
		if (!strcmp(tokens[0],"autores")){
			printf("Olalla Rama Garcia\n olalla.rama.garcia\n");
			printf("Juan Mosquera Cid\n juan.mosquera.cid\n");
		} 
		else if (!strcmp(tokens[0],"pid")){
			cmd_pid();
		}
		else if (!strcmp(tokens[0],"assign")){
			cmd_assign(&tokens[1]);
		}
		else if (!strcmp(tokens[0],"deassign")){
			cmd_deassign(&tokens[1]);
		}
		else if (!strcmp(tokens[0],"rmkey")){
			cmd_rmkey(tokens[1]);
		}
		else if (!strcmp(tokens[0],"mem")){
			cmd_mem();
		}
		else if (!strcmp(tokens[0],"memdump")){
			cmd_memdump(&tokens[1]);
		}
		else if (!strcmp(tokens[0],"recursiva")){
			Recursiva(tokens[1]);
		}
		else if (!strcmp(tokens[0],"iofile")){
			cmd_iofile(&tokens[1]);
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
		printf("$");
		fgets(cadena,MAXCADENA,stdin);
		//FGETS:lee una cadena s con i caracteres del archivo f
		//STDIN: limpia el buffer de entrada (por teclado,...)

		if (TrocearCadena(cadena,ctroceada)==0)
			continue; 

		ProcesarCadena(ctroceada, &acabar);
	}
}

