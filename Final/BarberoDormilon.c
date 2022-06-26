#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_CLIENTES 25

void *cliente(void *num);
void *barbero(void *num);

sem_t sillaEspera;
sem_t barberoSilla;
sem_t barberoDormir;
sem_t corteCabello;

int todosAtendidos = 0;
int main(int argC, char *argV[]) {
	pthread_t btid;
	pthread_t tid[MAX_CLIENTES];

	int numClientes, numSillas;
	int number[MAX_CLIENTES];

	if (argC != 3) {
		printf("Faltan comandos. Uso: BarberoDormilon <numero clientes> <numero sillas>\n");
		exit(EXIT_SUCCESS);
	}

	// Parámetros
	numClientes = atoi(argV[1]);
	numSillas = atoi(argV[2]);

	// Comprobar número de clientes
	if (numClientes > MAX_CLIENTES) {
		printf("El número de clientes debe ser menor a %d\n", MAX_CLIENTES);
		exit(EXIT_FAILURE);
	}

	// Inicializar arreglo
	for(int i = 0; i < MAX_CLIENTES; i++)
		number[i] = i;

	// Iniciar semáforos
	sem_init(&sillaEspera, 0, numSillas);
	sem_init(&barberoSilla, 0, 1);
	sem_init(&barberoDormir, 0, 0);
	sem_init(&corteCabello, 0, 0);

	// Barbero
	pthread_create(&btid, NULL, barbero, NULL);

	// Clientes
	for (int i = 0; i < numClientes; i++)
		pthread_create(&tid[i], NULL, cliente, (void *)&number[i]);

	// Esperar clientes
	for (int i = 0; i < numClientes; i++)
		pthread_join(tid[i], NULL);

	todosAtendidos = 1;
	// Despierta el barbero
	sem_post(&barberoDormir);
	pthread_join(btid, NULL);

}

void *cliente(void *number) {
	int num = *(int *) number;

	printf("El cliente %d llegó a la barbería\n", num + 1);

	// Espera para sentarse
	sem_wait(&sillaEspera);
	printf("El cliente %d está esperando\n", num + 1);

	// 
	sem_wait(&barberoSilla);

	// Cliente sale de espera
	sem_post(&sillaEspera);

	// Se levanta el barbero
	printf("El cliente %d despierta al barbero\n", num + 1);
	sem_post(&barberoDormir);

	// Termina el barbero
	sem_post(&barberoSilla);
	printf("El cliente %d pagó y se fué\n", num + 1);

	pthread_exit(0);
}

void *barbero(void *num) {
	while(!todosAtendidos) {
		printf("El barbero está durmiendo\n");
		sem_wait(&barberoDormir);
		if (!todosAtendidos) {
			printf("El barbero está cortando cabello\n");

			// Terminó de cortar el cabello
			sem_post(&corteCabello);
			printf("Ya terminó\n");
		}
	}
	printf("El barbero salió de la barbería\n");
	pthread_exit(0);
}
