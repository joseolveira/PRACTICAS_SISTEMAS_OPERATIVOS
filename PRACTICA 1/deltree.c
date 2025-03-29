#include "funciones.h"
int main (int argc, char *argv[]) {

	deltree(argv[1]);
	printf("Directorio %s eliminado satisfactoriamente.\n",argv[1]);
	return 0;
}
