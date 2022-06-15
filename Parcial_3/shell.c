// Programa shell

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>

#define tamañoEntrada 255
#define operadores "<>|"
#define cantidadComandos 10

typedef struct {
	char operador;
	char comando[tamañoEntrada / 2];
	int entradaFD;
	int salidaFD;
	int esArchivo;
} ejecucion;

typedef struct {
	ejecucion ops[cantidadComandos];
	int size;
} operaciones;

int obtencionOperaciones(operaciones *, char *);
char *trim(char *);

int main() {
	char comando[tamañoEntrada];
	do {
		printf(">$$ ");
		// Obtencioń del comando
		fgets(comando, 255, stdin);
		strcpy(comando, trim(comando));

		// Salir
		if (strcmp(comando, "exit") == 0)
			break;

		// Creación de memoria compartida
		int idMemoria;
		if ((idMemoria = shmget(IPC_PRIVATE, sizeof(operaciones), IPC_CREAT | 0600)) == -1) {
			perror("Error en shmget");
			exit(EXIT_FAILURE);
		}
		operaciones *ops = (operaciones *) shmat(idMemoria, 0, 0);

		// Manejo del comando ingresado
		obtencionOperaciones(ops, comando);

		// Crear tubería
		int tuberia[2];
		if (pipe(tuberia) == -1) {
			perror("Error al crear tuberia");
			exit(EXIT_FAILURE);
		}

		// Ejecución de comandos
		pid_t procesos[ops->size];
		int estado;

		// Actualizar entradas y salidas
		for (int i = 0; i < ops->size; i++) {
			int indiceComandoAnterior = -1;
			if (ops->ops[i].esArchivo == 1) {
				for(int j = i - 1; j >= 0; j--) {
					if(ops->ops[j].esArchivo == 0) {
						indiceComandoAnterior = j;
						break;
					}
				}
			} else
				indiceComandoAnterior = i;

			// Revisar operador
			switch(ops->ops[i].operador) {
				case '|':
					ops->ops[indiceComandoAnterior].salidaFD = tuberia[1];
					ops->ops[i + 1].entradaFD = tuberia[0];
					ops->ops[i + 1].salidaFD = 1;
					break;
				case '<':
					ops->ops[i + 1].esArchivo = 1;
					// Abrir archivo
					int entrada = open(ops->ops[i + 1].comando, O_RDONLY, 0600);
					ops->ops[i].entradaFD = entrada;
					ops->ops[i].salidaFD = 1;
					break;
				case '>':
					ops->ops[i + 1].esArchivo = 1;
					int salida = open(ops->ops[i + 1].comando, O_CREAT|O_WRONLY|O_TRUNC, 0600);
					ops->ops[indiceComandoAnterior].salidaFD = salida;
					break;
			}
		}

		// Ejecutar procesos
		for (int i = 0; i < ops->size; i++) {
			if (ops->ops[i].esArchivo == 1)
				continue;
			if ((procesos[i] = fork()) == 0) {
				// Cambiar entradas y salidas estandar
				dup2(ops->ops[i].entradaFD, 0);
				dup2(ops->ops[i].salidaFD, 1);
				close(tuberia[0]);
				close(tuberia[1]);

				// Ejecutar comando
				if (execlp(ops->ops[i].comando, ops->ops[i].comando, NULL) == -1) {
					printf("Ocurrió un Error");
				}
				exit(EXIT_SUCCESS);
			} else
				wait(&estado);
		}

		// Eliminar memoria compartida
		shmdt(ops);
	} while(1);

	exit(EXIT_SUCCESS);
}

void agregarOperacion(operaciones *ops, ejecucion nueva) {
	ops->ops[ops->size] = nueva;
	ops->size++;
}

int obtencionOperaciones(operaciones *ops, char *linea) {
	char *lineaAux = (char *) malloc(sizeof(char) * strlen(linea));
	strcpy(lineaAux, linea);
	ops->size = 0;

	// Obtener número de comandos y cadena
	char *string, *ptrSeguro, *comando;
	for (string = linea; ;string = NULL) {
		comando = strtok_r(string, operadores, &ptrSeguro);

		if (comando == NULL)
			break;

		if (strcmp(linea, "") != 0 && strcmp(linea, " ") != 0 && strcmp(linea, "  ") != 0) {
			// Guardar nuevo comando
			ejecucion nueva;
			strcpy(nueva.comando, comando);
			nueva.entradaFD = 0;
			nueva.salidaFD = 1;
			nueva.operador = ' ';
			nueva.esArchivo = 0;
			agregarOperacion(ops, nueva);
		}
	}

	if(!ops->size)
		return 0;

	// Obtener tipo de operaciones
	int index = -1;
	for (int i = 0; i < ops->size - 1; i++) {
		index += strlen(ops->ops[i].comando) + 1;

		// Obtener operador y limpiar comando
		ops->ops[i].operador = lineaAux[index];
		char *temp = strdup(ops->ops[i].comando);
		strcpy(ops->ops[i].comando, trim(temp));
	}
	// Último comando
	char *temp = strdup(ops->ops[ops->size - 1].comando);
	strcpy(ops->ops[ops->size - 1].comando, trim(temp));

	return 1;
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
