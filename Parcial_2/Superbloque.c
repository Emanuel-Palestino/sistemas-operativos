/*
 * 
 * Devuelve los mismos valores que se imprimen al ejecutar stat -f, recib
 * es para ejecutarlo es necesario escribir una ruta válida por linea de comandos.
 * CCBY: Cruz Villalba Roberto Carlos, Palestino Hernández Emanuel
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <string.h>

int main(int argC, char *argV[]){

    char *ruta;


    if (argC < 2) {
        perror("Faltan argumentos. Ingresar ruta.");
        exit(EXIT_FAILURE);
    }
    ruta = argV[1];
    if(strlen(ruta)>255){
        perror("Tamaño de ruta excedido. Tamaño máximo de ruta = 255 caracteres");
        exit(EXIT_FAILURE);
    }


    struct statvfs vfs;
    if(statvfs(ruta,&vfs)!=0){
        perror("Llamado de statvfs");
        //perror(1);
    }


	printf("   Fichero: \"%s\"\n  ", ruta);

    printf("ID del Dispositivo: %#lx ", (unsigned long) vfs.f_fsid);
	printf("Long. nombre: %ld    ", (long) vfs.f_namemax);
    //Buscar  tipo
    printf("Tipo:\n");

	printf("Tam. bloque: %ld     ", (long) vfs.f_bsize);
	printf("Tam. bloque fundamental: %ld\n", (long) vfs.f_frsize);

    printf("Bloques: ");
	printf("Total: %lu\t", (unsigned long) vfs.f_blocks);
	printf("Libres: %lu\t", (unsigned long) vfs.f_bfree);
	printf("Disponibles: %lu\n", (unsigned long) vfs.f_bavail);

	printf("Inodos: ");
    printf("Total: %lu\t", (unsigned long) vfs.f_files);
	printf("Libres: %lu\n", (unsigned long) vfs.f_ffree);
	
	


    return 0;
}