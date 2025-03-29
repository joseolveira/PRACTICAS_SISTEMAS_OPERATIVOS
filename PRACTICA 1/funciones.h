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

char TipoFichero(mode_t m);
int TrocearCadena(char * cadena, char * tokens[]);
void deltree (char * dir ) ;
void list(char *cadenarecibida[]);
