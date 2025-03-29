#include "funciones.h"

#define MAXCADENA 2048

char * ConvierteModo (mode_t m, char *permisos)
{
   strcpy (permisos,"---------- ");
   permisos[0]=TipoFichero(m);
   if (m&S_IRUSR) permisos[1]='r';  //S_IRUSR: permiso de lectura, propietario
   if (m&S_IWUSR) permisos[2]='w';  //S_IWUSR: permiso de escritura, propiestario
   if (m&S_IXUSR) permisos[3]='x';  //S_IXUSR: permiso de ejecución/búsqueda, propietario
   if (m&S_IRGRP) permisos[4]='r';  //S_IRGRP: permiso de lectura, grupo
   if (m&S_IWGRP) permisos[5]='w';  //S_IWGRP: permiso de escritura, grupo
   if (m&S_IXGRP) permisos[6]='x';  //S_IXGRP: permiso de ejecución/búsqueda, grupo
   if (m&S_IROTH) permisos[7]='r';  //S_IROTH: permiso de lectura, otros
   if (m&S_IWOTH) permisos[8]='w';  //S_IWOTH: permiso de escritura, otros
   if (m&S_IXOTH) permisos[9]='x';  //S_IXOTH: permiso de ejecución/búsqueda, otros
   if (m&S_ISUID) permisos[3]='s';  //S_ISUID: set-user-ID on execution
   if (m&S_ISGID) permisos[6]='s';  //S_ISGID: set-group-ID on execution
   if (m&S_ISVTX) permisos[9]='t';  //S_ISVTX: on directories, restricted deletion flag
   return permisos;
}

int TrocearCadena(char * cadena, char * tokens[]){ //Se pone * porque si tokens es un puntero cadena tambien tiene que ser de tipo modificable y 
												   // para eso tiene que tener el * porque sino no se ppuede modificar
	int i=1;
	if ((tokens[0]=strtok(cadena," \n\t"))==NULL){
		/*STRTOK: Permite convertir una cadena en subcadenas pudiendo decidir cual
	      es el separador. En este caso ' ', \n y \t.
	      Como esta funcion ya guarda la cadena, la segunda vez que la usemos le hemos
	      de pasar NULL en vez de la cadena.*/
		return 0;
	}
	while ((tokens[i]=strtok(NULL," \n\t"))!=NULL){ //Se le pasa nulo porque ya guarda la cadena la primera vez
		i++;
	}
	return i;//trozos que hizo la cadena
}

char TipoFichero (mode_t m){
	switch (m&S_IFMT) { 		      //and bit a bit con los bits de formato,0170000 
		case S_IFSOCK: return 's';    //socket 
		case S_IFLNK:  return 'l';    //symbolic link
        case S_IFREG:  return '-';    //fichero normal
		case S_IFBLK:  return 'b';    //block device
		case S_IFDIR:  return 'd';    //directorio 
		case S_IFCHR:  return 'c';    //char device
		case S_IFIFO:  return 'p';    //pipe
		default:       return '?';    //desconocido, no deberia aparecer
	}
}

void imprimirDatos(struct stat info){
	struct tm *time;
	struct passwd *pw;
	struct group  *gr;
	char fecha[20];
	
	pw = getpwuid(info.st_uid);
	gr = getgrgid(info.st_gid);
	time = localtime(&info.st_mtime);
	printf("%ld",(long)info.st_ino);
	printf("\t");
	printf( (S_ISDIR(info.st_mode)) ? "d" : "-");
	printf( (info.st_mode & S_IRUSR) ? "r" : "-");
	printf( (info.st_mode & S_IWUSR) ? "w" : "-");
	printf( (info.st_mode & S_IXUSR) ? "x" : "-");
	printf( (info.st_mode & S_IRGRP) ? "r" : "-");
	printf( (info.st_mode & S_IWGRP) ? "w" : "-");
	printf( (info.st_mode & S_IXGRP) ? "x" : "-");
	printf( (info.st_mode & S_IROTH) ? "r" : "-");
	printf( (info.st_mode & S_IWOTH) ? "w" : "-");
	printf( (info.st_mode & S_IXOTH) ? "x" : "-"); 
	printf(" %ld",(long)info.st_nlink);
	printf(" %s %s ", pw->pw_name, gr->gr_name);
	printf("%ld",(long)info.st_size);
	printf("\t");
	strftime(fecha, 50, "%b %d %H:%M ", time);
	printf("%s", fecha);
}

void Listar(char *dir){
	
	struct stat info;
	struct dirent *direntp;// va a servir para hacer un opendir. Es un puntero que apunta a la estructura dirent
	char ruta[MAXCADENA];
	int vacio=1;
		
	DIR *dirp;//se crea dirp de tipo directorio(DIR)
	dirp = opendir(dir);	//opendir de la ruta(dir).

	if(dirp == NULL) {	//No se puede abrir el directorio.
		perror("Error");
		exit(1);
	}
	
	printf("\n");
	strcat(dir,"/");
	printf("%s:\n",dir);
	printf("\n");
	while((direntp = readdir(dirp)) != NULL){
		if(direntp->d_name[0]==46)	//direntp->d_name son las entradas que contiene el directorio(es decir, subdirectorio, archivos, ...)
			continue;
		vacio=0;
		strcpy(ruta, dir);
		strcat(ruta, direntp->d_name);
		stat(ruta,&info);	//Le añade a info toda la informacion pertinente de ruta(permisos, tipo de fichero, ...)
		
		imprimirDatos(info);
		printf("%s", direntp->d_name);
		printf("\n");	
	}
	if(vacio)
		printf("No hay nada en el directorio.\n");
	closedir(dirp);
}

void ListadoRecursivo(char *d){
	mode_t mode;
 	struct stat info;	
	struct dirent *direntp;	
	char ruta[MAXCADENA];
	char r[MAXCADENA];
	char aux[20][MAXCADENA];	//[20] elementos del array de tipo string.(es el maximo de subdirectorios)
	
	int a=0;
	int b=0;
	int vacio=1;
	

	DIR *dirp;
	dirp = opendir(d);

	if(dirp == NULL) {
		perror("Error");
		exit(1);
	}
	
	printf("\n");
	strcat(d,"/");
	printf("%s:\n",d);
	while((direntp = readdir(dirp)) != NULL){
		if(direntp->d_name[0]==46)
			continue;
		vacio=0;	
		strcpy(ruta, d);
		strcat(ruta, direntp->d_name);
		stat(ruta,&info);
			
		mode = info.st_mode;	//Modo del archivo que estoy leyendo.
			
		if(TipoFichero(mode)=='d'){
			sprintf(aux[a], "%s", direntp->d_name);	//guarda en aux lo que hay en direntp->d_name.
			a++;	
		}
		
		imprimirDatos(info);
		printf("%s", direntp->d_name);
		printf("\n");
	}
	if(vacio)
		printf("No hay nada en el directorio.\n");	
	closedir(dirp);
	for(b=0; b<a; b++){
		strcpy(r, d);
		strcat(r, aux[b]);	//Le concateno a r(que es la ruta que le pasa list a ListadoRecursivo) el primer subdirectorio
							//que guardé en el array de 20 elementos.
		ListadoRecursivo(r);
	}
}

void list(char *cadenarecibida[]){
	char dir[MAXCADENA];
	int x=0, r=0;
	
	if (getcwd(dir,MAXCADENA)==NULL) //getcwd obtiene la ruta actual
	  	perror("Error");

	if(cadenarecibida[x]==NULL){	//Caso en el que solo introduzco list y por lo tanto me muestra el directorio actual.
		printf("\n");
		printf("Listar: %s\n",dir);
		Listar(dir); //lista el directorio actual
		printf("\n");
	}
	while(cadenarecibida[x]!=NULL){
		if(!strcmp(cadenarecibida[x], "-r") || !strcmp(cadenarecibida[x], "-R"))	//Evalua si el usuario piede listar de forma recursiva.
			r=1;	
		else if(cadenarecibida[x][0]==47)//0 es el primer bit. 47  en codigo ascci es /
			strcat(dir,cadenarecibida[x]); //copia en dir cadena recibida solo si "Prueba" tiene una / al principio.
		else {
			if (getcwd(dir,MAXCADENA)==NULL)	//Si no tiene la / se la asignamos aquí.
	  			perror("Error");
			strcat(dir, "/");
			strcat(dir, cadenarecibida[x]);
		}
		
		if((strcmp(cadenarecibida[x], "-r") && strcmp(cadenarecibida[x], "-R"))!=0){	//Le envía la ruta que queremos mostrar
																						//a su procedimiento adecuado en función 
																						//de si es recursivo o no.
			if(!r){
				printf("\n");
				printf("Listar: %s\n", cadenarecibida[x]);
				Listar(dir);
				printf("\n");
			}
			else{
				printf("\n");
				printf("Listar [-r]: %s\n", cadenarecibida[x]);
				ListadoRecursivo(dir);
				printf("\n");
				r=0;
			}
		}
		x++;
	}
}

void deltree (char * dir ) {
	
	char localBuffer[MAXCADENA];
	DIR *dirp;
	struct dirent *direntp; //*direntp:apunta a una variable que es un registro. dirent contiene a d_name
	//como direntp apunta a dirent ya se omite porque cuando nombremos a direntp ya estamos hablando de dirent.
	struct stat buf;
	    
	dirp = opendir(dir);
	if (dirp == NULL){
		perror("Error");
		exit(1);
	}
	else{
		while ((direntp = readdir(dirp)) != NULL) {
			if( (strcmp(direntp->d_name, ".")!=0) && (strcmp(direntp->d_name, "..")!=0) ){
				strcpy(localBuffer,dir);
				//strcpy: string copy
				strcat(localBuffer,"/");
				//strcat:concatena
				strcat(localBuffer,direntp->d_name);//concatena el directorio siguiente. Es decir si le pasamos prueba y dentro hay prueba1
				                                    //y prueba2 aparecerá Prueba/Prueba1
				if ((lstat (localBuffer,&buf)) < 0) { //lstat: para comprobar si es archivo, directorio...para los permisos
					                                  //guarda en buf todo lo referente a localBuffer
													  //si es cero suponemos que no fue capaz de realizarlo y dará error
					perror("Error");
				}	
				printf("Borrando %s\n",localBuffer);
				if (TipoFichero(buf.st_mode) == 'd') { //para saber si es un directorio
					deltree(localBuffer);  //hace llamada a deltree y le pasamos localBuffer para comprobar si hay algo dentro
				}
				else {
					if (remove(localBuffer) < 0) { //si no es un directorio hace remove de localBuffer
						perror("Error");
					}	
				}
			}
		}
	}
	if (closedir(dirp) < 0) { //cierra el directorio que abrimos con dirp. Si da -1 da error. 0 es correcto
		perror("Error");
	}	
	if (remove (dir) < 0) {
		perror("Error");
	}	
}
