// Banquero

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SEMAFORO_CLIENTE 0
#define SEMAFORO_BANCO 1
#define NUM_MAX_CLIENTES 5

typedef struct mem {
	int turno;
	int numeroTransaccion;
	int prestamoClientes[NUM_MAX_CLIENTES];
	int ultimaPeticion;
} memoria;

int esHijo(pid_t, pid_t *, int);
void cerrarSemaforo(int, struct sembuf *, int);
void abrirSemaforo(int, struct sembuf *, int);
FILE *abrirArchivo(char *, char *);
void abrirArchivoUltimaLinea(FILE *, char *, char *);

int main(int argC, char *argV[]) {
	// Creación de la llave
	key_t llave;
	llave = ftok(argV[0], 'e');

	// Creación y configuración de los semaforos
	int idSemaforo;
	struct sembuf operacion;
	if ((idSemaforo = semget(llave, 2, IPC_CREAT | 0600)) == -1) {
		perror("Error en semget");
		exit(EXIT_FAILURE);
	}
	semctl(idSemaforo, SEMAFORO_CLIENTE, SETVAL, 1);
	semctl(idSemaforo, SEMAFORO_BANCO, SETVAL, 0);


	// Creación de memoria compartida
	int idMemoria;
	if ((idMemoria = shmget(IPC_PRIVATE, sizeof(memoria), IPC_CREAT | 0600)) == -1) {
		perror("Error en shmget");
		exit(EXIT_FAILURE);
	}
	memoria *global = (memoria *) shmat(idMemoria, 0, 0);

	// Saber qué cliente va a iniciar
	global->numeroTransaccion = 0;
	global->turno = 0;

	// Obtener Información de los clientes
	FILE *archivo;

	// Leer capital
	int capital;
	archivo = abrirArchivo("banco.txt", "r");
	fscanf(archivo, "%d", &capital);

	// Obtener Clientes
	int numeroClientes;
	fscanf(archivo, "%d", &numeroClientes);

	// Leer necesidad de clientes
	int necesidadClientes[numeroClientes];
	int demandaClientes[numeroClientes];
	for (int i = 0; i < numeroClientes; i++)
		fscanf(archivo, "%d ", &necesidadClientes[i]);

	fclose(archivo);

	// Creación de clientes
	pid_t clientes[numeroClientes];
	for (int i = 0; i < numeroClientes; i++) {
		if ((clientes[i] = fork()) == -1) {
			perror("Error en la creación del hijo");
			exit(EXIT_FAILURE);
		}

		if (clientes[i] == 0) {
			// Acciones del cliente
			while(global->numeroTransaccion < 3) {
				if (i == global->turno) {
					cerrarSemaforo(idSemaforo, &operacion, SEMAFORO_CLIENTE);
					printf("Cliente %d haciendo petición...\n", i);
	
					archivo = abrirArchivo("banco.txt", "a");

					// Imprimir número de transaccción
					global->numeroTransaccion++;
					fprintf(archivo, "%d - ", global->numeroTransaccion);
	
					// Imprimir prestamo del cliente
					for (int j = 0; j < numeroClientes; j++) {
						if (global->numeroTransaccion == 1)
							global->prestamoClientes[j] = 0;

						if (j == global->turno)
							global->prestamoClientes[j]++;
						fprintf(archivo, "%d ", global->prestamoClientes[j]);
					}
					fprintf(archivo, "- ");

					// Imprimir necesidades de los clientes
					for (int j = 0; j < numeroClientes; j++)
						fprintf(archivo, "%d ", necesidadClientes[j]);
					fprintf(archivo, "- ");

					// Imprimir demanda del cliente
					for (int j = 0; j < numeroClientes; j++)
						fprintf(archivo, "%d ", (necesidadClientes[j] - global->prestamoClientes[j]));
					
					fclose(archivo);

					printf("... Petición terminada.\n\n");
					global->ultimaPeticion = global->turno;
					global->turno = -1;

					abrirSemaforo(idSemaforo, &operacion, SEMAFORO_BANCO);
				}
			}

			exit(EXIT_SUCCESS);
		}
	}

	// Acciones del banco
	while(global->numeroTransaccion < 3) {
		cerrarSemaforo(idSemaforo, &operacion, SEMAFORO_BANCO);
	
		// Obtener el siguiente cliente a pedir
		global->turno = global->numeroTransaccion;

		printf("Banco evaluando petición #%d de %d:\n", global->numeroTransaccion, global->ultimaPeticion);

		// Leer archivo en última línea
		archivo = abrirArchivo("banco.txt", "a+");
		int num = 1;
		fseek(archivo, -num, SEEK_END);
		char c = getc(archivo);
		while(c != '\n') {
			num++;
			fseek(archivo, -num, SEEK_END);
			c = getc(archivo);
		}
	
		// acciones - leer del archivo y comprobar estado seguro
		int nada;
		fscanf(archivo, "%d - ", &nada);
	
		// Obtener Prestamo
		for (int i = 0; i < numeroClientes; i++)
			fscanf(archivo, "%d ", &global->prestamoClientes[i]);
		fscanf(archivo, " - ");
	
		// Obtener Necesidad
		for (int i = 0; i < numeroClientes; i++)
			fscanf(archivo, "%d ", &nada);
		fscanf(archivo, " - ");
	
		// Obtener Demanda
		for (int i = 0; i < numeroClientes; i++)
			fscanf(archivo, "%d", &demandaClientes[i]);
	
		// comprobar estado
		int estado = 1;
		if (estado) {
			fprintf(archivo, "- Seguro\n");
		} else {
			fprintf(archivo, "- No Seguro\n");
		}

		fclose(archivo);

		// abrir semaforo cliente
		printf("... Evaluación terminada.\n\n");
		abrirSemaforo(idSemaforo, &operacion, SEMAFORO_CLIENTE);

	}

	// Borrar semaforo
	semctl(idSemaforo, 0, IPC_RMID, 0);

	// Cerrar y Borrar Memoria Compartida
	shmdt(global);
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

FILE *abrirArchivo(char *nombre, char *modo) {
	FILE *temporal;
	if ((temporal = fopen(nombre, modo)) == NULL) {
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}
	return temporal;
}
