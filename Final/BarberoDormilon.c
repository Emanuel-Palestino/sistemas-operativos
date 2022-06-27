#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_CLIENTES 30
#define SILLAS_ESPERA 4

void *cliente(void *numero);
void *barbero(void *numero);

sem_t barberoListo;
sem_t sillasEspera;
sem_t sillaBarbero;
sem_t clientes;

int hayClientes = 1;

int main(int argC, char *argV[]) {
	pthread_t barberoHilo;
	pthread_t clientesHilos[MAX_CLIENTES];

	int numClientes;

	if (argC != 2) {
		printf("Faltan comandos. Para ejecutar: BarberoDormilon <numero clientes>\n");
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
	sem_init(&sillaBarbero, 0, 0);
	sem_init(&sillasEspera, 0, SILLAS_ESPERA);
	sem_init(&clientes, 0, 0);

	// Barbero
	pthread_create(&barberoHilo, NULL, barbero, NULL);

	int numC[MAX_CLIENTES];
	for (int i = 0; i < MAX_CLIENTES; i++)
		numC[i] = i + 1;

	// Clientes
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	for (int i = 0; i < numClientes; i++)
		pthread_create(&clientesHilos[i], &attr, cliente, &numC[i]);

	// Esperar clientes
	for (int i = 0; i < numClientes; i++)
		pthread_join(clientesHilos[i], NULL);

	hayClientes = 0;
	sem_post(&clientes);

	pthread_join(barberoHilo, NULL);
	exit(EXIT_SUCCESS);
}

void *cliente(void *numero) {
	int numeroCliente = *(int *) numero;

	sem_wait(&sillasEspera);
	printf("Cliente %d esperando al barbero\n", numeroCliente);
	sem_post(&clientes);

	sem_wait(&barberoListo);
	printf("BARBERO cortando el cabello al cliente %d\n", numeroCliente);
	sem_post(&sillaBarbero);

	pthread_exit(0);
}

void *barbero(void *numero) {
	while(hayClientes) {
		printf("BARBERO DURMIENDO\n");
		sem_wait(&clientes);
		if (!hayClientes)
			break;
		printf("BARBERO DESPIERTO\n");
		sem_post(&barberoListo);

		sem_wait(&sillaBarbero);
		printf("BARBERO TERMINÓ DE CORTAR\n");

		sem_post(&sillasEspera);
	}
	printf("---El barbero cierra la barbería (se queda dormido)---\n");
	pthread_exit(0);
}
