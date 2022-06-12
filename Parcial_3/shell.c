// Programa shell

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define tamañoEntrada 255
#define operadores "<>|"

typedef struct {
	char operador;
	char comando[tamañoEntrada / 2];
	int entradaFD;
	int salidaFD;
} ejecucion;

typedef struct {
	ejecucion *ops;
	int size;
} operaciones;

operaciones *obtencionOperaciones(char *);
char *trim(char *);

int main() {
	char comando[tamañoEntrada], some;
	do {
		printf(">$$ ");
		// Obtencioń del comando
		fgets(comando, 255, stdin);
		strcpy(comando, trim(comando));

		// Manejo de la cadena
		operaciones *ops;
		ops = obtencionOperaciones(comando);

		// Imprimir operaciones
		for (int i = 0; i < ops->size; i++) {
			printf("comando: %s operador: %c ", ops->ops[i].comando, ops->ops[i].operador);
		}

		if (ops->size)
			printf("\n");

	} while(strcmp(comando, "exit") != 0);

	exit(EXIT_SUCCESS);
}

void agregarOperacion(operaciones *ops, ejecucion nueva) {
	ejecucion *aux;
	aux = ops->ops;
	ops->size++;
	printf("antes\n");
	ops->ops = (ejecucion *) malloc(sizeof(ejecucion) * ops->size);
	printf("despues\n");
	for (int i = 0; i < ops->size - 1; i++)
		ops->ops[i] = aux[i];
	ops->ops[ops->size - 1] = nueva;
	free(aux);
}

operaciones *obtencionOperaciones(char *linea) {
	char *lineaAux = (char *) malloc(sizeof(char) * strlen(linea));
	strcpy(lineaAux, linea);
	operaciones *ops = (operaciones *) malloc(sizeof(operaciones));
	ops->ops = NULL;

	// Obtener número de comandos y cadena
	char *string, *ptrSeguro, *comando;
	for (string = linea; ;string = NULL) {
		comando = strtok_r(string, operadores, &ptrSeguro);
		if (comando == NULL)
			break;
		if (strcmp(linea, "") != 0 && strcmp(linea, " ") != 0 && strcmp(linea, "  ") != 0) {
			ejecucion nueva;
			strcpy(nueva.comando, comando);
			nueva.entradaFD = 0;
			nueva.salidaFD = 1;
			nueva.operador = ' ';
			agregarOperacion(ops, nueva);
		}
	}

	if(!ops->size)
		return ops;

	// Obtener tipo de operaciones
	int index = -1;
	for (int i = 0; i < ops->size - 1; i++) {
		index += strlen(ops->ops[i].comando) + 1;
		ops->ops[i].operador = lineaAux[index];
		strcpy(ops->ops[i].comando, trim(ops->ops[i].comando));
	}
	strcpy(ops->ops[ops->size - 1].comando, trim(ops->ops[ops->size - 1].comando));

	return ops;
}

char *trim(char *str) {
	char *end;

	// Trim leading space
	while(isspace((unsigned char)*str)) str++;

	if(*str == 0)  // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace((unsigned char)*end)) end--;

	// Write new null terminator character
	end[1] = '\0';

	return str;
}
