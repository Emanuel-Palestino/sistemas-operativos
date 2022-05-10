// Banquero

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>

#define SEMAFORO_CLIENTE 0
#define SEMAFORO_BANCO 1

int esHijo(pid_t, pid_t *, int);
void cerrarSemaforo(int, struct sembuf *, int);
void abrirSemaforo(int, struct sembuf *, int);
void abrirArchivo(FILE *, char *, char *);
void abrirArchivoUltimaLinea(FILE *, char *, char *);

int main(int argC, char *argV[]) {
	int idSemaforo;
	struct sembuf operacion;
	key_t llave;

	// Creación del semaforo
	llave = ftok(argV[0], 'e');
	if ((idSemaforo = semget(llave, 2, IPC_CREAT | 0600)) == -1) {
		perror("Error en semget");
		exit(EXIT_FAILURE);
	}

	// Creación de memoria compartida
	int idMemoria;
	if ((idMemoria = shmget(IPC_PRIVATE, 4, IPC_CREAT | 0600)) == -1) {
		perror("Error en shmget");
		exit(EXIT_FAILURE);
	}
	char *turno = (char *) shmat(idMemoria, 0, 0);
	strcpy(turno, "jeje");
	printf("Memoria 1: %s\n", turno);

	// Configuración del semaforo
	semctl(idSemaforo, SEMAFORO_CLIENTE, SETVAL, 1);
	semctl(idSemaforo, SEMAFORO_BANCO, SETVAL, 0);

	// Obtener Capital
	int capital;
	FILE *archivo;
	if ((archivo = fopen("banco.txt", "r")) == NULL) {
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	fscanf(archivo, "%d", &capital);

	// Obtener Clientes
	int numeroClientes;
	fscanf(archivo, "%d", &numeroClientes);
	// Leer necesidad de clientes
	int necesidadClientes[numeroClientes];
	for (int i = 0; i < numeroClientes; i++)
		fscanf(archivo, "%d ", &necesidadClientes[i]);

	fclose(archivo);

	// Creación de hijos
	pid_t clientes[numeroClientes];
	for (int i = 0; i < numeroClientes; i++) {
		if ((clientes[i] = fork()) == -1) {
			perror("Error en la creación del hijo");
			exit(EXIT_FAILURE);
		}

		if (clientes[i] == 0) {
			// Acciones del cliente
			if (i == 0) {
				cerrarSemaforo(idSemaforo, &operacion, SEMAFORO_CLIENTE);

				if ((archivo = fopen("banco.txt", "a")) == NULL) {
					perror("Error al abrir el archivo");
					exit(EXIT_FAILURE);
				}
				// Imprimir necesidades del cliente
				fprintf(archivo, "1 - 1 0 0 - 4 6 8 - 3 6 8");
				fclose(archivo);

				strcpy(turno, "jojo");

				abrirSemaforo(idSemaforo, &operacion, SEMAFORO_BANCO);
			}

			// Borrar semaforo
			//semctl(idSemaforo, 0, IPC_RMID, 0);
			exit(EXIT_SUCCESS);
		}
	}

	// cerramos semáforo del banco
	cerrarSemaforo(idSemaforo, &operacion, SEMAFORO_BANCO);

	// Acciones del banco
	if ((archivo = fopen("banco.txt", "a+")) == NULL) {
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	int num = 1;
	fseek(archivo, -num, SEEK_END);
	char c;
	c = getc(archivo);
	while(c != '\n') {
		num++;
		fseek(archivo, -num, SEEK_END);
		c = getc(archivo);
	}

	// Quitamos número de transacción
	int *nada = (int *) malloc(sizeof(int *));
	fscanf(archivo, "%d - ", nada);

	
	// acciones - leer del archivo y comprobar estado seguro
	int prestamoClientes[numeroClientes];
	int demandaClientes[numeroClientes];

	// prestamo
	for (int i = 0; i < numeroClientes; i++) {
		fscanf(archivo, "%d ", &prestamoClientes[i]);
		printf("%d\n", prestamoClientes[i]);
	}
	fscanf(archivo, " - ");

	// necesidad
	for (int i = 0; i < numeroClientes; i++)
		fscanf(archivo, "%d ", nada);
	fscanf(archivo, " - ");

	// demanda
	for (int i = 0; i < numeroClientes; i++) {
		fscanf(archivo, "%d", &demandaClientes[i]);
		printf("%d\n", demandaClientes[i]);
	}

	// comprobar estado
	int estado = 1;

	if (estado) {
		fprintf(archivo, " - Seguro\n");
	} else {
		fprintf(archivo, " - No Seguro\n");
	}

	// test memoria
	printf("Memoria: %s\n", turno);
	

	fclose(archivo);
	// abrir semaforo cliente
	abrirSemaforo(idSemaforo, &operacion, SEMAFORO_CLIENTE);

	// Borrar semaforo
	semctl(idSemaforo, 0, IPC_RMID, 0);

	shmdt(turno);
	shmctl(idMemoria, IPC_RMID, 0);
	exit(EXIT_SUCCESS);

}

void cerrarSemaforo(int id, struct sembuf *operacion, int semaforo) {
	operacion->sem_num = semaforo;
	operacion->sem_op = -1;
	operacion->sem_flg = 0;
	semop(id, operacion, 1);
}

void abrirSemaforo(int id, struct sembuf *operacion, int semaforo) {
	operacion->sem_num = semaforo;
	operacion->sem_op = 1;
	operacion->sem_flg = 0;
	semop(id, operacion, 1);
}

void abrirArchivo(FILE *arc, char *nombre, char *modo) {
	if ((arc = fopen("banco.txt", "r")) == NULL) {
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}
}

void abrirArchivoUltimaLinea(FILE *archivo, char *nombre, char *modo) {
	if ((archivo = fopen(nombre, modo)) == NULL) {
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}
	int num = 1;
	fseek(archivo, -num, SEEK_END);
	char c;
	c = getc(archivo);
	while(c != '\n') {
		num++;
		fseek(archivo, -num, SEEK_END);
		c = getc(archivo);
	}
}
