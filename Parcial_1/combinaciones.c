#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int esHijo(pid_t, pid_t *, int);
int main(int argC, char *argV[])
{

    int estado;

    if (argC < 2)
    {
        perror("Faltan argumentos. Uso: ./combinaciones n k");
        exit(EXIT_FAILURE);
    }

    int n = atoi(argV[1]);
    int k = atoi(argV[2]);
    pid_t hijos[k + 1];

    printf("Padre: %ld\n", (long)getpid());

    for (int i = k; i >= 0; i--)
    {
        // Padre
        //        if (hijo = fork())
        //            break;
        if ((hijos[i] = fork()) == -1)
        {
            perror("Error al crear el hijo");
            exit(EXIT_FAILURE);
        }

        // Hijo
        if (hijos[i] == 0)
        {
            printf("Hijo numero: %d. PID: %ld. PPID: %ld\n", i, (long)getpid(), (long)getppid());
            for (int j = 0; j < 1000; j++)
                ;
            // exit(EXIT_SUCCESS);
            return 0;
        }
    }

    // Padre
    printf("Voy a empezar a esperar\n");
    for (int i = k; i >= 0; i--)
    {
        if (esHijo(wait(&estado), hijos, k + 1))
        {
            printf("soy el papa de %d, que acaba de terminar\n", hijos[i]);
        }
        else
        {
            printf("Hijo finalizado anormalmente\n");
            // exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}

int esHijo(pid_t hijo, pid_t hijos[], int tamaño)
{

    for (int i = 0; i < tamaño; i++)
    {
        if (hijo == hijos[i])
            return 1;
    }
    return 0;
}