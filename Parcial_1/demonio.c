#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int estado;
    pid_t hijo;

    if ((hijo = fork()) == -1) {
        perror("Error al crear hijo");
        exit(EXIT_FAILURE);
    }

    if (hijo == 0) {
        printf("Hijo\n");
        setsid();
        while(1) {
            printf("SOY EL DEMONIO Y TENGO EL PID: %ld ASOCIADO.\n", (long) getpid());
            sleep(30);
        }
    }

    exit(EXIT_SUCCESS);
}
