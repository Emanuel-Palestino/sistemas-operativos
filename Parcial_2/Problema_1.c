// Problema 1
// Calcular el determinante de una matriz 2x2
// Calcular el determinante de una matriz 3x3
// Calcular el determinante de una matriz 4x4

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <errno.h>

int **matriz = NULL;
int cola;

typedef struct mensaje {
	long tipo;
	int i;
	int j;
	char texto[80];
} mensaje;

typedef struct propiedadesHilo {
	int tipo;
	int resultado;
} pHilo;

// Función de lee una matriz desde un archivo
int **leerMatriz(FILE *, char *, int *);
void *resolucion(void *);
void enviarMensaje(int, int, mensaje);
void recibirMensaje(int, int, mensaje *);

int main(int argC, char *argV[]) {
	// Leer matriz del archivo
	FILE *archivoMatriz = NULL;
	int tamaño = 0;
	matriz = leerMatriz(archivoMatriz, "matriz.txt", &tamaño);

	// Cola mensajes
	int llave;
	llave = ftok(argV[0], 'e');
	if ((cola = msgget(llave, IPC_CREAT | 0666)) == -1) {
		perror("Error al crear la cola");
		exit(EXIT_FAILURE);
	}

	mensaje msjTest;
	enviarMensaje(cola, 2, msjTest);
	// Mensajes iniciales (cofactores)
	for (int i = 0; i < tamaño; i++) {
		mensaje msj;
		msj.i = i;
		enviarMensaje(cola, 1, msj);
	}

	// Hilos
	pthread_t hilos[tamaño];
	pthread_attr_t attr;

	// Cofactores Iniciales
	for (int i = 0; i < tamaño; i++) {
		pHilo ph;
		ph.tipo = 1;
		pthread_attr_init(&attr);
		pthread_create(&hilos[i], &attr, resolucion, &ph);
	}

	// Esperar hilos
	for (int i = 0; i < tamaño; i++) {
		pthread_join(hilos[i], NULL);
	}

	exit(EXIT_SUCCESS);
}

void *resolucion(void *ph) {
	pHilo p = *((pHilo *) ph);
	mensaje m;
	recibirMensaje(cola, p.tipo, &m);
	pthread_exit(0);
}

void enviarMensaje(int idCola, int tipoMensaje, mensaje msj) {
	// Crear mensaje
	msj.tipo = tipoMensaje;

	// Enviar mensaje
	if (msgsnd(idCola, (void *) &msj, sizeof(msj.i), IPC_NOWAIT) == -1) {
		perror("Error al enviar mensaje");
		exit(EXIT_FAILURE);
	}
}

void recibirMensaje(int idCola, int tipoMensaje, mensaje *msj) {
	// Recibir mensaje
	if (msgrcv(idCola, (void *) msj, sizeof(msj->i), tipoMensaje, MSG_NOERROR|IPC_NOWAIT) == -1) {
		if (errno != ENOMSG) {
			perror("Error al recibir mensaje");
			exit(EXIT_FAILURE);
		}

		printf("No hay mensaje para leer\n");
	} else
		printf("Mensaje recibido: %d\n", msj->i);
}

int **leerMatriz(FILE *archivo, char *nombre, int *tamaño) {
	// Leer archivo
	if ((archivo = fopen(nombre, "r")) == NULL) {
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	// Obtener matriz
	fscanf(archivo, "%d", tamaño);

	int **matriz = (int **) malloc(*tamaño * sizeof(int *));

	for (int i = 0; i < *tamaño; i++) {
		matriz[i] = (int *) malloc(*tamaño * sizeof(int));
		for (int j = 0; j < *tamaño; j++)
			fscanf(archivo, "%d", &matriz[i][j]);
	}

	return matriz;
}
