#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>

#define RUTA 255

int main(void) {
	char ruta [RUTA + 1] = "/proc";
	DIR *dir;
	struct dirent *direntrada;
	char rutaStat[128];
	

	printf("Ruta actual: %s\n", ruta);

	if ((dir = opendir(ruta)) == NULL) {
		perror("No puedo leer el directorio");
	}

	while((direntrada = readdir(dir)) != NULL) {
		if(direntrada->d_name[0]>47 && direntrada->d_name[0]<58){
		sprintf(rutaStat,"/proc/%s/stat",direntrada->d_name);
		FILE *fd = fopen(rutaStat,"r");
		if(fd==NULL)
		perror("Error al abrir archivo\n");
		char nombre[32];int pid;

		fscanf(fd,"%d %s",&pid,nombre);
		close(fd);


		if((direntrada->d_type == DT_DIR)){
			printf("PID: %s ",direntrada->d_name);
		}
		printf("Nombre Proceso: %s\n",nombre);
		}
	}

	closedir(dir);


	exit(EXIT_SUCCESS);
}
