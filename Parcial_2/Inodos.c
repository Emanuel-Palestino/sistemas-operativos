/*
 * 
 * Ejecuta la una función igual al comando "ls -l" a nivel de impresión, 
 * sin embargo al encontrar una carpeta, imprime el contenido de la misma de manera recursiva y así
 * sucesivamente para todas las carpetas encontradas.
 * CCBY: Cruz Villalba Roberto Carlos, Palestino Hernández Emanuel
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define RUTA 255

void recorrerCarpeta(char*,int);
int main(void) {
    char ruta [RUTA + 1];

	if (getcwd(ruta, RUTA + 1) == NULL) {
		perror("No se tuvo acceso al directorio");
		exit(EXIT_FAILURE);
	}
    recorrerCarpeta(ruta,0);

	exit(EXIT_SUCCESS);
    }


void recorrerCarpeta(char* ruta, int tabuladores){
    struct stat sb;
	printf("Ruta actual: %s\n", ruta);

	DIR *dir;
	struct dirent *direntrada;
	if ((dir = opendir(ruta)) == NULL) {
		perror("No puedo leer el directorio");
	}

	while((direntrada = readdir(dir)) != NULL) {
		char rutaAux[2*RUTA];
		sprintf(rutaAux,"%s/%s",ruta,direntrada->d_name);
        //obtener stat de archivo
		if((lstat(rutaAux, &sb))== -1){
			printf("Nombre del archivo: %s", direntrada->d_name);
			perror("Error en lstat");
			exit(EXIT_FAILURE);
		}
		
        //identificar si es una carpeta
        if(S_ISDIR(sb.st_mode) ){
			if(strlen(direntrada->d_name) >= 1 && direntrada->d_name[0] != '.'){
				if(strlen(direntrada->d_name) >= 2 && direntrada->d_name[0] != '.' && direntrada->d_name[1] != '.'){
					printf("\n%s es una carpeta Válida\n",direntrada->d_name);		
            		recorrerCarpeta(rutaAux,tabuladores+1);
				}
			}
		}

		/*impresión de formato ls*/
		for(int i=0; i<tabuladores ; i++)
			printf("\t");

		printf("Nombre del Archivo: %s\n", direntrada->d_name);


	}
	closedir(dir);
}