/*
 * 
 * Calcula y muestra la sumatoria de un conjunto de n elementos tomando k de ellos. Desde k hasta 0
 *
 * CCBY: Palestino Hernández Emanuel
 * Licencia: Apache 2.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Función para encontrar coincidencia de un hijo en un arreglo.
// Como parámetros recibe el hijo, el apuntador al arreglo de hijos y el tamaño del arreglo
// Devuelve el indice del arreglo donde se encuentra el hijo o -1 si no lo encontró
int esHijo(pid_t, pid_t *, int);
// Calcula el factorial de un número
int factorial(int);
// Calcula las combinaciones de n en k
int combinaciones(int, int);

int main(int argC, char *argV[]) {
    if (argC < 2) {
        perror("Faltan argumentos. Uso: ./Combinaciones n k");
        exit(EXIT_FAILURE);
    }

    int estado;
    int n = atoi(argV[1]);
    int k = atoi(argV[2]);
    int resultados[k + 1];
    pid_t hijos[k + 1];
    // Comunicaciones
    int fd[k + 1][2];

    for (int i = k; i >= 0; i--) {
        // Creación de la comunicacion
        if (pipe(fd[i]) < 0) {
            perror("Error en pipe");
            exit(EXIT_FAILURE);
        }

        // Creación del hijo
        if ((hijos[i] = fork()) == -1) {
            perror("Error al crear el hijo");
            exit(EXIT_FAILURE);
        }

        // Hijo
        if (hijos[i] == 0) {
            char resultado[10];
            int res = combinaciones(n, i);
            printf("Combinacion n:%d en k:%d, = %d\n", n, i, res);

            // Mandar el resultado
            sprintf(resultado, "%d", res);
            close(fd[i][0]);
            write(fd[i][1], resultado, 10);
            
            exit(EXIT_SUCCESS);
        }
    }

    // Padre
    for (int i = k; i >= 0; i--) {
        char resultado[10];
        int posicionHijo = esHijo(wait(&estado), hijos, k + 1);
        if (posicionHijo >= 0) {
            // Leer el resultado
            close(fd[posicionHijo][1]);
            read(fd[posicionHijo][0], resultado, 5);
            resultados[posicionHijo] = atoi(resultado);
        } else
            printf("Hijo finalizado anormalmente\n");
    }

    // Sumar y mostrar el resultado
    int resultadoFinal = 0;
    for (int i = 0; i < k + 1; i++)
        resultadoFinal += resultados[i];

    printf("\nResultado Final = %d\n", resultadoFinal);
    return EXIT_SUCCESS;
}

int esHijo(pid_t hijo, pid_t *hijos, int tamaño) {
    for (int i = 0; i < tamaño; i++) {
        if (hijo == hijos[i])
            return i;
    }
    return -1;
}

int factorial(int numero) {
    if (numero == 1 || numero == 0)
        return 1;
    return numero * factorial(numero -1);
}

int combinaciones(int n, int k) {
    return factorial(n) / (factorial(n - k) * factorial(k));
}
