#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main() {
    pid_t hijo;
    int fdPadre[2], fdHijo[2], fdEM[2];
    char mensaje[255] = {'H', 'o', 'l', 'a', '!', '\n', 0};

    if (pipe(fdPadre) < 0) {
        perror("Error al crear la tubería");
        exit(EXIT_FAILURE);
    }

    if (pipe(fdHijo) < 0) {
        perror("Error al crear la tubería");
        exit(EXIT_FAILURE);
    }

    if (pipe(fdEM) < 0) {
        perror("Error al crear la tubería");
        exit(EXIT_FAILURE);
    }

    if ((hijo = fork()) == -1) {
        perror("Error al crear el hijo");
        exit(EXIT_FAILURE);
    }

    // Padre
    if (hijo != 0) {
        printf("---Inicio de Conversación---\n\n");
    } else {
        close(fdPadre[0]);
        write(fdPadre[1], mensaje, 255);

    }

    while(strncmp(mensaje, "mata kondo", 10) != 0) {

        // Padre
        if (hijo != 0) {
            close(fdPadre[1]);
            read(fdPadre[0], mensaje, 255);

            if (strncmp(mensaje, "mata kondo", 10) == 0)
                exit(EXIT_SUCCESS);

            printf("\n----------------------------------------------\n");
            printf("\nInvitado 2 dijo: %s", mensaje);
            printf("Mensaje a Invitado 2:\n");
            fgets(mensaje, 255, stdin);
            close(fdHijo[0]);
            write(fdHijo[1], mensaje, 255);

        } else {
            // Hijo
            close(fdHijo[1]);
            read(fdHijo[0], mensaje, 255);

            if (strncmp(mensaje, "mata kondo", 10) == 0)
                exit(EXIT_SUCCESS);

            printf("\n----------------------------------------------\n");
            printf("\nInvitado 1 dijo: %s", mensaje);
            printf("Mensaje a Invitado 1:\n");
            fgets(mensaje, 255, stdin);
            close(fdPadre[0]);
            write(fdPadre[1], mensaje, 255);
        }
    }

    exit(EXIT_SUCCESS);

}

