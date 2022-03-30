/*
*****************************Hcombinaciones.c*****************************************
**                                                                                  **
**  Descipción: Este programa calcula un conjunto de combinatorias mediante un hilo **
** y las suma en el padre. Calcula la combinatoria de k en n y la suma de todas las **
** combinatorias de k para todas las n decrementándolas hasta llegar a 0            **
**                                                                                  **
**  CCBY: Cruz Villalba Roberto Carlos                                              **
**  Grupo: 602-A        Materia: Sistemas Operativos                                **
**  UNIVERSIDAD TECNOLÓGICA DE LA MIXTECA                                           **
**  Licencia: libre                                                                 **
**************************************************************************************
*/

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int factorial(int);
int combinaciones(int, int);

typedef struct {
    pthread_t tId;
    int identificador;
    int n;
    int k;
    int resultadoCombinatoria;
} hilo;

void *funcionHilo (void *hilo);

int main(int argC, char *argV[]) {

    if (argC < 2) {
        perror("Faltan argumentos. Uso: ./combinaciones n k");
        exit(EXIT_FAILURE);
    }

    int n = atoi(argV[1]);
    int k = atoi(argV[2]);

    printf("n: %d\n",n);
    printf("k: %d\n",k);

    pthread_attr_t attr;
	hilo hilos[n];
    
    
    //crea k hilos
    for (int i = k; i >= 0 ; i--) {
 		pthread_attr_init(&attr);
         hilos[i].n=n;
         hilos[i].k=i;
 		pthread_create(&hilos[i].tId, &attr, funcionHilo, &hilos[i]);
	}

    // Sumar y mostrar el resultado
    int resultadoFinal = 0;
    //espera que cada hilo termine
    for (int i = 0; i < n+1; i++){
 		pthread_join(hilos[i].tId, NULL);
         resultadoFinal += hilos[i].resultadoCombinatoria;
    }

    

    printf("\nResultado Final = %d\n", resultadoFinal);

    return EXIT_SUCCESS;
}

void *funcionHilo (void *hil) {
    hilo *hi = (hilo*) hil;
    hi->identificador = pthread_self();
    hi->resultadoCombinatoria = combinaciones(hi->n, hi->k);
    printf("Combinacion n:%d en k:%d, = %d\n", hi->n, hi->k, hi->resultadoCombinatoria );
    pthread_exit(0);
}

int factorial(int numero) {
    if (numero == 1 || numero == 0)
        return 1;
    return numero * factorial(numero -1);
}

int combinaciones(int n, int k) {
    return factorial(n) / (factorial(n - k) * factorial(k));
}

