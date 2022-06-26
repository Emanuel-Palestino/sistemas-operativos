#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_CLIENTES 25
#define SILLAS_ESPERA 4

void *cliente(void *num);
void *barbero(void *num);

sem_t barberoListo;
sem_t sillasAccesibles;
sem_t clientes;

int sillasLibres = SILLAS_ESPERA;
int todosAtendidos = 0;

int main(int argC, char *argV[]) {
	pthread_t barberoHilo;
	pthread_t clientesHilos[MAX_CLIENTES];

	int numClientes;

	if (argC != 2) {
		printf("Faltan comandos. Uso: BarberoDormilon <numero clientes>\n");
		exit(EXIT_SUCCESS);
	}

	// Numeros de clientes
	numClientes = atoi(argV[1]);

	// Comprobar número de clientes
	if (numClientes > MAX_CLIENTES) {
		printf("El número de clientes debe ser menor a %d\n", MAX_CLIENTES);
		exit(EXIT_FAILURE);
	}

	// Iniciar semáforos
	sem_init(&barberoListo, 0, 0);
	sem_init(&sillasAccesibles, 0, 1);
	sem_init(&clientes, 0, 0);

	// Barbero
	pthread_create(&barberoHilo, NULL, barbero, NULL);

	int numC[MAX_CLIENTES];
	for (int i = 0; i < MAX_CLIENTES; i++)
		numC[i] = i;

	// Clientes
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	for (int i = 0; i < numClientes; i++)
		pthread_create(&clientesHilos[i], &attr, cliente, &numC[i]);

	// Esperar clientes
	for (int i = 0; i < numClientes; i++)
		pthread_join(clientesHilos[i], NULL);

	todosAtendidos = 1;
	// Despierta el barbero
	sem_post(&barberoListo);

	pthread_join(barberoHilo, NULL);

	exit(EXIT_SUCCESS);
}

void *cliente(void *number) {
	int num = *(int *) number;

	sem_wait(&sillasAccesibles);
	if (sillasLibres > 0) {
		sillasLibres--;
		printf("Cliente %d esperando\n", num);
		sem_post(&clientes);
		sem_post(&sillasAccesibles);
		sem_wait(&barberoListo);
		printf("Cortando el cabello al cliente %d\n", num);
	} else {
		sem_post(&sillasAccesibles);
		printf("El cliente %d se va porque no hay sillas para esperar\n", num);
	}

	pthread_exit(0);
}

void *barbero(void *num) {
	while(!todosAtendidos) {
		printf("El barbero está durmiendo\n");
		sem_wait(&clientes);
		sem_wait(&sillasAccesibles);
		sillasLibres++;
		sem_post(&barberoListo);
		sem_post(&sillasAccesibles);
	}
	printf("---El barbero cierra la barbería---\n");
	pthread_exit(0);
}
