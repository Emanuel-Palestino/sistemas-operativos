// Problema 2
// Calcular el determinante de una matriz 2x2
// Calcular el determinante de una matriz 3x3
// Calcular el determinante de una matriz 4x4

#include <stdio.h>
#include <stdlib.h>

int** leerMatriz(FILE *, char *, int *);

int main() {
	// Leer matriz del archivo
	FILE *archivoMatriz = NULL;
	int tamaño = 0;
	int **matriz = leerMatriz(archivoMatriz, "matriz.txt", &tamaño);

	for (int i = 0; i < tamaño; i++) {
		for (int j = 0; j < tamaño; j++) {
			printf("%d ", matriz[i][j]);
		}
		printf("\n");
	}
}

int** leerMatriz(FILE *archivo, char *nombre, int *tamaño) {
	if ((archivo = fopen(nombre, "r")) == NULL) {
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	// Obtener matriz
	fscanf(archivo, "%d", tamaño);

	int **matriz = (int **) malloc(*tamaño * sizeof(int *));

	for (int i = 0; i < *tamaño; i++) {
		matriz[i] = (int *) malloc(*tamaño * sizeof(int));
		for (int j = 0; j < *tamaño; j++) {
			fscanf(archivo, "%d", &matriz[i][j]);
		}
	}

	return matriz;
}
