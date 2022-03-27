#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	pthread_t tId;
    int numeroHilo;
    int identificador;
} hilo;

void *funcionHilo (void *hilo);

int main(int argC, char *argV[]){
    int n = strtol(argV[1], NULL, 10);

    if (argC < 2) {
  		fprintf(stderr, "Uso: ./hilos [enteros]\n");
  		return -1;
 	}

    pthread_attr_t attr;
	hilo hilos[n-1];

    //crea n hilos
    for (int i = 0; i < n ; i++) {
 		// Coloca atributo predeterminados o coloque NULL
 		pthread_attr_init(&attr);
		hilos[i].numeroHilo=i;
 		pthread_create(&hilos[i].tId, &attr, funcionHilo, &hilos[i]);
	}
    for (int i = 0; i < n; i++){
 		pthread_join(hilos[i].tId, NULL);
 		printf("Termino el hilo con id = %u\n", (unsigned int) hilos[i].identificador);
    }


    return EXIT_SUCCESS;
}

void *funcionHilo (void *hil) {
    hilo *hi = (hilo*) hil;
    hi->identificador = pthread_self();
    printf("soy el hilo: %d\n",hi->numeroHilo + 1);
 	pthread_exit(0);
}