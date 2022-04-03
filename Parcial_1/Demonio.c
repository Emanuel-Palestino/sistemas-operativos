/*
 * 
 * Crea un demonio que cada 30 segundos escribe en un archivo
 *
 * CCBY: Palestino Hernández Emanuel
 * Licencia: Apache 2.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t hijo;
    FILE *archivo;

    if ((hijo = fork()) == -1) {
        perror("Error al crear hijo");
        exit(EXIT_FAILURE);
    }

    if (hijo == 0) {
        setsid();
        while(1) {
            archivo = fopen("demonio.txt", "a");
            fprintf(archivo, "SOY EL DEMONIO Y TENGO EL PID: %ld ASOCIADO.\n", (long) getpid());
            sleep(30);
            fclose(archivo);
        }
    }

    exit(EXIT_SUCCESS);
}
