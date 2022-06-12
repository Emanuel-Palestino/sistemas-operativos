// Programa shell

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
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
		if (strcmp(comando, "exit") == 0)
			break;

		operaciones *ops;

		// Manejo de la cadena
		ops = obtencionOperaciones(comando);

		// Ejecutar comandos
		int tuberia[2];
		if (pipe(tuberia) == -1) {
			perror("Error al crear tuberia");
			exit(EXIT_FAILURE);
		}
		pid_t procesos[ops->size];
		int estado;
		for (int i = 0; i < ops->size - 1; i++) {
			if ((procesos[i] = fork()) == 0) {
				// Revisar operador derecho
				switch(ops->ops[i].operador) {
					case '|':
						ops->ops[i].entradaFD = 0;
						ops->ops[i].salidaFD = tuberia[1];
						break;
				}
				// Revisar operador izquierdo
				if (i > 0) {
					switch(ops->ops[i - 1].operador) {
						case '|':
							ops->ops[i].entradaFD = tuberia[0];
							break;
					}
				}
				dup2(ops->ops[i].entradaFD, 0);
				dup2(ops->ops[i].salidaFD, 1);
				close(tuberia[0]);
				close(tuberia[1]);
				execlp(ops->ops[i].comando, ops->ops[i].comando, NULL);
				exit(EXIT_SUCCESS);
			} else {
				wait(&estado);
			}
		}
		if ((procesos[ops->size - 1] = fork()) == 0) {
			// Revisar operador izquierdo
			if (ops->size > 1) {
				switch(ops->ops[ops->size - 2].operador) {
					case '|':
						ops->ops[ops->size - 1].entradaFD = tuberia[0];
						break;
				}
			}
			dup2(ops->ops[ops->size - 1].entradaFD, 0);
			dup2(ops->ops[ops->size - 1].salidaFD, 1);
			close(tuberia[0]);
			close(tuberia[1]);
			execlp(ops->ops[ops->size - 1].comando, ops->ops[ops->size - 1].comando, "shell.c", NULL);
			exit(EXIT_SUCCESS);
		} else {
			wait(&estado);
		}

	} while(1);

	exit(EXIT_SUCCESS);
}

void agregarOperacion(operaciones *ops, ejecucion nueva) {
	ejecucion *aux;
	aux = ops->ops;
	ops->size++;
	ops->ops = (ejecucion *) malloc(sizeof(ejecucion) * ops->size);
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
	char *temp = strdup(ops->ops[ops->size - 1].comando);
	strcpy(ops->ops[ops->size - 1].comando, trim(temp));

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
