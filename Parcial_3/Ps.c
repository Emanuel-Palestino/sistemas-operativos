#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>

#define RUTA 255

int main(void) {
    //se crea la variable ruta par leer el directorio proc
	char ruta [RUTA + 1] = "/proc";
	DIR *dir;
	struct dirent *direntrada;
	char rutaStat[128];
	

	printf("Ruta actual: %s\n", ruta);

	if ((dir = opendir(ruta)) == NULL) {
		perror("No puedo leer el directorio");
	}

    //Lectura de proc
	while((direntrada = readdir(dir)) != NULL) {
        //cuando el primer caracter del nombre del archivo que se está leyendo es un número, se asume que es un proceso
        //segun el codigo ascii, si está en el rango de ser algun numero se abre el archivo stat que está en /proc/nombreCarpeta/stat
        //abrimos el archivo, lo leemos y obtenemos los primeros dos campos del documento. Estos son el PID y el nombre del archivo
		if(direntrada->d_name[0]>47 && direntrada->d_name[0]<58){
		sprintf(rutaStat,"/proc/%s/stat",direntrada->d_name);
		FILE *fd = fopen(rutaStat,"r");
		if(fd==NULL)
		perror("Error al abrir archivo\n");
		char nombre[32];int pid;

        //guardamos el nombre del archivo en la variable nombre
		fscanf(fd,"%d %s",&pid,nombre);
		close(fd);

        //si el archivo que se está leyendo es una carpeta, imprimiremos el nombre de la carpeta que ya de por si es el PID
        //y después imprimiremos el nombre obtenido
		if((direntrada->d_type == DT_DIR)){
			printf("PID: %s ",direntrada->d_name);
		}
		printf("Nombre Proceso: %s\n",nombre);
		}
	}

	closedir(dir);


	exit(EXIT_SUCCESS);
}
