// Banquero

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SEMAFORO_CLIENTE 0
#define SEMAFORO_BANCO 1

typedef struct mem {
	int turno;
	int numeroTransaccion;
} memoria;

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
	if ((idMemoria = shmget(IPC_PRIVATE, sizeof(memoria), IPC_CREAT | 0600)) == -1) {
		perror("Error en shmget");
		exit(EXIT_FAILURE);
	}
	memoria *global = (memoria *) shmat(idMemoria, 0, 0);

	// Configuración del semaforo
	semctl(idSemaforo, SEMAFORO_CLIENTE, SETVAL, 1);
	semctl(idSemaforo, SEMAFORO_BANCO, SETVAL, 0);

	// Saber qué cliente va a iniciar
	global->turno = 0;
	global->numeroTransaccion = 0;

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

	int prestamoClientes[numeroClientes];
	int demandaClientes[numeroClientes];

	// Creación de hijos
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
					printf("cliente %d:\n", i);
	
					fpos_t pos;
					if ((archivo = fopen("banco.txt", "a+")) == NULL) {
						perror("Error al abrir el archivo");
						exit(EXIT_FAILURE);
					}
					int num = 2;
					fseek(archivo, -num, SEEK_END);
					char c;
					c = getc(archivo);
					while(c != '\n') {
						num++;
						fseek(archivo, -num, SEEK_END);
						c = getc(archivo);
					}
					fgetpos(archivo, &pos);
					// Obtener número de transaccción
					global->numeroTransaccion++;
					int nada;
					if (global->numeroTransaccion > 1) {
						fsetpos(archivo, &pos);
						fscanf(archivo, "%d - ", &nada);
						fgetpos(archivo, &pos);
						printf("num tran. anterior: %d\n", nada);
					}
					fseek(archivo, 0, SEEK_END);
					fprintf(archivo, "%d - ", global->numeroTransaccion);
	
					// Imprimir prestamo del cliente
					for (int j = 0; j < numeroClientes; j++) {
						if (global->numeroTransaccion > 1) {
							fsetpos(archivo, &pos);
							fscanf(archivo, "%d ", &prestamoClientes[j]);
							fgetpos(archivo, &pos);
							printf("%d ", prestamoClientes[j]);
						}
						else
							prestamoClientes[j] = 0;
						if (j == global->turno)
							prestamoClientes[j]++;
						fseek(archivo, 0, SEEK_END);
						fprintf(archivo, "%d ", prestamoClientes[j]);
					}
					if (global->numeroTransaccion > 1) {
						fsetpos(archivo, &pos);
						fscanf(archivo, " - ");
						fgetpos(archivo, &pos);
					}
					fseek(archivo, 0, SEEK_END);
					fprintf(archivo, "- ");
					// Imprimir necesidades de los clientes
					for (int j = 0; j < numeroClientes; j++) {
						if (global->numeroTransaccion > 1) {
							fsetpos(archivo, &pos);
							fscanf(archivo, "%d ", &necesidadClientes[j]);
							fgetpos(archivo, &pos);
						}
						fseek(archivo, 0, SEEK_END);
						fprintf(archivo, "%d ", necesidadClientes[j]);
					}
					if (global->numeroTransaccion > 1) {
						fsetpos(archivo, &pos);
						fscanf(archivo, " - ");
						fgetpos(archivo, &pos);
					}
					fseek(archivo, 0, SEEK_END);
					fprintf(archivo, "- ");
					// Imprimir demanda del cliente
					for (int j = 0; j < numeroClientes; j++) {
						fseek(archivo, 0, SEEK_END);
						fprintf(archivo, "%d ", (necesidadClientes[j] - prestamoClientes[j]));
					}
					
					fclose(archivo);

					printf("\n%d...\n", i);
					global->turno = -1;
					abrirSemaforo(idSemaforo, &operacion, SEMAFORO_BANCO);
				}
			}

			exit(EXIT_SUCCESS);
		}
	}

	// Acciones del banco

	while(global->numeroTransaccion < 3) {
		// cerramos semáforo del banco
		cerrarSemaforo(idSemaforo, &operacion, SEMAFORO_BANCO);
	
		// Obtener el siguiente cliente a pedir
		global->turno = global->numeroTransaccion;

		printf("banco %d:\n", global->numeroTransaccion);
		// Leer archivo en última línea
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
	
		// acciones - leer del archivo y comprobar estado seguro
		int nada;
		fscanf(archivo, "%d - ", &nada);
	
		// Obtener Prestamo
		for (int i = 0; i < numeroClientes; i++) {
			fscanf(archivo, "%d ", &prestamoClientes[i]);
		}
		fscanf(archivo, " - ");
	
		// Obtener Necesidad
		for (int i = 0; i < numeroClientes; i++)
			fscanf(archivo, "%d ", &nada);
		fscanf(archivo, " - ");
	
		// Obtener Demanda
		for (int i = 0; i < numeroClientes; i++) {
			fscanf(archivo, "%d", &demandaClientes[i]);
		}
	
		// comprobar estado
		int estado = 1;
	
		if (estado) {
			fprintf(archivo, "- Seguro\n");
		} else {
			fprintf(archivo, "- No Seguro\n");
		}

		fclose(archivo);

		// abrir semaforo cliente
		printf("banco %d...\n", global->numeroTransaccion);
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
