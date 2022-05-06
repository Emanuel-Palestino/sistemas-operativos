// Problema 1

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <errno.h>

int **matriz = NULL;
int cola;

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
void enviarMensaje(int, mensaje);
void recibirMensaje(int, int, mensaje *);
int **obtenerMenor(int **, int, int, int);
int resolver2x2(int **);
int potencia(int, int);

int main(int argC, char *argV[]) {
	// Leer matriz del archivo
	FILE *archivoMatriz = NULL;
	int tamaño = 0;
	matriz = leerMatriz(archivoMatriz, "matriz4.txt", &tamaño);

	// Cola mensajes
	int llave;
	llave = ftok(argV[0], 'e');
	if ((cola = msgget(llave, IPC_CREAT | 0666)) == -1) {
		perror("Error al crear la cola");
		exit(EXIT_FAILURE);
	}

	// Mensajes iniciales (Cofactor a calcular)
	for (int i = 0; i < tamaño; i++) {
		mensaje msj;
		msj.tipo = tamaño - 1;
		msj.contenido.factor = potencia(-1, i) * matriz[0][i];
		msj.contenido.mat = obtenerMenor(matriz, tamaño, 0, i);
		enviarMensaje(cola, msj);
	}

	// Hilos
	pthread_t hilos[tamaño];
	pthread_attr_t attr;

	// Procesos que resuelven cofactores
	pHilo resultadoCofactores[tamaño];
	for (int i = 0; i < tamaño; i++) {
		resultadoCofactores[i].tipo = tamaño - 1;
		pthread_attr_init(&attr);
		pthread_create(&hilos[i], &attr, resolucion, &resultadoCofactores[i]);
	}

	// Esperar hilos
	for (int i = 0; i < tamaño; i++)
		pthread_join(hilos[i], NULL);

	// Calcular resultado final (suma de cofactores)
	long resultadoFinal = 0;
	for (int i = 0; i < tamaño; i++)
		resultadoFinal += resultadoCofactores[i].resultado;

	printf("Resultado de la Determinante = %ld\n", resultadoFinal);

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
		// Enviar a cola Cofactor a calcular
		for (int i = 0; i < tamaño; i++) {
			mensaje msj;
			msj.tipo = tamaño - 1;
			msj.contenido.factor = m.contenido.factor * potencia(-1, i) * m.contenido.mat[0][i];
			msj.contenido.mat = obtenerMenor(m.contenido.mat, tamaño, 0, i);
			enviarMensaje(cola, msj);
		}

		// Hilos
		pthread_t hilos[tamaño];
		pthread_attr_t attr;

		// Procesos que resuelven cofactores
		pHilo resultadoCofactores[tamaño];
		for (int i = 0; i < tamaño; i++) {
			resultadoCofactores[i].tipo = tamaño - 1;
			pthread_attr_init(&attr);
			pthread_create(&hilos[i], &attr, resolucion, &resultadoCofactores[i]);
		}

		// Esperar hilos
		for (int i = 0; i < tamaño; i++)
			pthread_join(hilos[i], NULL);

		// Calcular resultado final (suma de cofactores)
		long resultado = 0;
		for (int i = 0; i < tamaño; i++)
			resultado += resultadoCofactores[i].resultado;
		p->resultado = resultado;
	}
	pthread_exit(0);
}

void enviarMensaje(int idCola, mensaje msj) {
	// Enviar mensaje
	if (msgsnd(idCola, (void *) &msj, sizeof(msj.contenido), IPC_NOWAIT) == -1) {
		perror("Error al enviar mensaje");
		exit(EXIT_FAILURE);
	}
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
	return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
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
	// Abrir archivo
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
