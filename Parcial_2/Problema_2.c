// Problema 2
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

typedef struct mensaje {
	long tipo;
	int i;
	int j;
	char texto[80];
} mensaje;

// Función de lee una matriz desde un archivo
int **leerMatriz(FILE *, char *, int *);
void *resolucion(void *);
void enviarMensaje(int, int);
void recibirMensaje(int, int);

int main(int argC, char *argV[]) {
	// Leer matriz del archivo
	FILE *archivoMatriz = NULL;
	int tamaño = 0;
	matriz = leerMatriz(archivoMatriz, "matriz.txt", &tamaño);

	// Hilos
	pthread_t hilo1;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_create(&hilo1, &attr, resolucion, NULL);

	// Cola mensajes
	int llave;
	llave = ftok(argV[0], 'e');
	int cola;
	if ((cola = msgget(llave, IPC_CREAT | 0666)) == -1) {
		perror("Error al crear la cola");
		exit(EXIT_FAILURE);
	}

	// Prueba mensajes
	enviarMensaje(cola, 2);
	recibirMensaje(cola, 1);
	recibirMensaje(cola, 2);
	recibirMensaje(cola, 2);

	// Esperar hilos
	pthread_join(hilo1, NULL);

	exit(EXIT_SUCCESS);
}

void *resolucion(void *) {
	pthread_exit(0);
}

void enviarMensaje(int idCola, int tipoMensaje) {
	// Crear mensaje
	mensaje msj;
	msj.tipo = tipoMensaje;
	snprintf(msj.texto, sizeof(msj.texto), "El mensaje es de tipo %d", tipoMensaje);

	// Enviar mensaje
	if (msgsnd(idCola, (void *) &msj, sizeof(msj.texto), IPC_NOWAIT) == -1) {
		perror("Error al enviar mensaje");
		exit(EXIT_FAILURE);
	}
}

void recibirMensaje(int idCola, int tipoMensaje) {
	mensaje msj;
	// Recibir mensaje
	if (msgrcv(idCola, (void *) &msj, sizeof(msj.texto), tipoMensaje, MSG_NOERROR|IPC_NOWAIT) == -1) {
		if (errno != ENOMSG) {
			perror("Error al recibir mensaje");
			exit(EXIT_FAILURE);
		}

		printf("No hay mensaje para leer\n");
	} else
		printf("Mensaje recibido: %s\n", msj.texto);
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
