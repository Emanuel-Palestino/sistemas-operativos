// Problema 1

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <errno.h>
#include <semaphore.h>

int **matriz = NULL;
int cola;
sem_t semaforo;

typedef struct contenido {
	int **mat;
	int factor;
} cont;

typedef struct mensaje {
	long tipo;
	cont contenido;
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
int **obtenerMenor(int **, int, int, int);
int resolver2x2(int **);
void imprimirMatriz(int **, int);
int potencia(int, int);

int main(int argC, char *argV[]) {
	// Leer matriz del archivo
	FILE *archivoMatriz = NULL;
	int tamaño = 0;
	matriz = leerMatriz(archivoMatriz, "matriz6.txt", &tamaño);

	// Cola mensajes
	int llave;
	llave = ftok(argV[0], 'e');
	if ((cola = msgget(llave, IPC_CREAT | 0666)) == -1) {
		perror("Error al crear la cola");
		exit(EXIT_FAILURE);
	}

	// Inicialización del semaforo
	sem_init(&semaforo, 0, 1);

	// Mensajes iniciales (Cofactor a calcular)
	for (int i = 0; i < tamaño; i++) {
		mensaje msj;
		msj.tipo = tamaño - 1;
		msj.contenido.factor = potencia(-1, i) * matriz[0][i];
		msj.contenido.mat = obtenerMenor(matriz, tamaño, 0, i);
		enviarMensaje(cola, msj.tipo, msj);
	}

	// Hilos
	pthread_t hilos[tamaño];
	pthread_attr_t attr;

	// Procesos que resuelven cofactores
	pHilo resultadoMenores[tamaño];
	for (int i = 0; i < tamaño; i++) {
		resultadoMenores[i].tipo = tamaño - 1;
		pthread_attr_init(&attr);
		pthread_create(&hilos[i], &attr, resolucion, &resultadoMenores[i]);
	}

	// Esperar hilos
	for (int i = 0; i < tamaño; i++) {
		pthread_join(hilos[i], NULL);
	}

	// Calcular resultado final (suma de cofactores)
	long resultadoFinal = 0;
	for (int i = 0; i < tamaño; i++) {
		resultadoFinal += resultadoMenores[i].resultado;
		//printf("res cofactor = %d\n", resultadoMenores[i].resultado);
	}

	printf("Resultado Final = %ld\n", resultadoFinal);

	exit(EXIT_SUCCESS);
}

void *resolucion(void *ph) {
	pHilo *p = (pHilo *) ph;
	mensaje m;
	recibirMensaje(cola, p->tipo, &m);
	if (m.tipo == 2) {
		p->resultado = m.contenido.factor * resolver2x2(m.contenido.mat);
	} else {
		int tamaño = m.tipo;
		// Mensajes iniciales (Cofactor a calcular)
		for (int i = 0; i < tamaño; i++) {
			mensaje msj;
			msj.tipo = tamaño - 1;
			msj.contenido.factor = m.contenido.factor * potencia(-1, i) * m.contenido.mat[0][i];
			msj.contenido.mat = obtenerMenor(m.contenido.mat, tamaño, 0, i);
			enviarMensaje(cola, msj.tipo, msj);
		}

		// Hilos
		pthread_t hilos[tamaño];
		pthread_attr_t attr;

		// Procesos que resuelven cofactores
		pHilo resultadoMenores[tamaño];
		for (int i = 0; i < tamaño; i++) {
			resultadoMenores[i].tipo = tamaño - 1;
			pthread_attr_init(&attr);
			pthread_create(&hilos[i], &attr, resolucion, &resultadoMenores[i]);
		}

		// Esperar hilos
		for (int i = 0; i < tamaño; i++) {
			pthread_join(hilos[i], NULL);
		}

		// Calcular resultado final (suma de cofactores)
		long resultadoFinal = 0;
		for (int i = 0; i < tamaño; i++) {
			resultadoFinal += resultadoMenores[i].resultado;
		}
		p->resultado = resultadoFinal;
	}
	pthread_exit(0);
}

void enviarMensaje(int idCola, int tipoMensaje, mensaje msj) {
	// Crear mensaje
	msj.tipo = tipoMensaje;

	// Enviar mensaje
	if (msgsnd(idCola, (void *) &msj, sizeof(msj.contenido), IPC_NOWAIT) == -1) {
		perror("Error al enviar mensaje");
		exit(EXIT_FAILURE);
	}
	//printf("mensaje enviado.\n");
}

void recibirMensaje(int idCola, int tipoMensaje, mensaje *msj) {
	// Recibir mensaje
	if (msgrcv(idCola, (void *) msj, sizeof(msj->contenido), tipoMensaje, MSG_NOERROR|IPC_NOWAIT) == -1) {
		if (errno != ENOMSG) {
			perror("Error al recibir mensaje");
			exit(EXIT_FAILURE);
		}

		printf("No hay mensaje para leer\n");
	} else {
		// Mensaje recibido
	}
}

int resolver2x2(int **mat) {
	int resultado = 0;
	resultado = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	return resultado;
}

int **obtenerMenor(int **matrizCompleta, int tam, int i, int j) {
	int **resultado = (int **) malloc((tam - 1) * sizeof(int *));
	int x = 0;
	for (int k = 0; k < tam; k++, x++) {
		if (k == i) {
			x--;
			continue;
		}
		int y = 0;
		resultado[x] = (int *) malloc((tam - 1) * sizeof(int));
		for (int l = 0; l < tam; l++, y++) {
			if (l == j) {
				y--;
				continue;
			}
			resultado[x][y] = matrizCompleta[k][l];
		}
	}

	return resultado;
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

void imprimirMatriz(int **mat, int tam) {
	for (int i = 0; i < tam; i++) {
		for (int j = 0; j < tam; j++) {
			printf("%d ", mat[i][j]);
		}
		printf("\n");
	}
}

int potencia(int base, int potencia) {
	if (potencia == 0)
		return 1;
	int resultado = base;
	while(potencia - 1) {
		resultado *= base;
		potencia--;
	}

	return resultado;
}
