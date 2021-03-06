/*
 * Terminal.c
 *
 * Forma de compilar: gcc Terminal.c -o Terminal
 * 
 * Programa que manda mensajes a todos los usuarios conectados en las diferentes tty's
 * indicando que su sesión se cerrará en x tiempo. Una vez terminado el tiempo,
 * la sesión de los usuarios se terminará.
 *
 * CCBY: Palestino Hernández Emanuel
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <utmp.h>
#include <time.h>
#include <signal.h>

#define TIEMPO 20
#define INTERVALO 2

typedef struct sesion {
	int pid;
	char tty[UT_LINESIZE];
} sesion;

int main() {
	struct utmp *usuario;
	int usuarios = 0;

	// Obtener cantidad de usuarios
	setutent();
	while((usuario = getutent())) {
		if (usuario->ut_type == USER_PROCESS && usuario->ut_line[0] == 't' && usuario->ut_line[0] == 't')
			usuarios++;
	}

	// Obtener información de usuarios
	sesion login[usuarios];
	int i = 0;
	setutent();
	while((usuario = getutent())) {
		if (usuario->ut_type == USER_PROCESS && usuario->ut_line[0] == 't' && usuario->ut_line[0] == 't') {
			login[i].pid = usuario->ut_pid;
			sprintf(login[i].tty, "%s", usuario->ut_line);
			i++;
		}
	}

	// Enviar mensajes usuarios
	FILE *tty;
	char ruta[100];

	int tiempo = TIEMPO;
	while(tiempo > 0) {
		// Obtener la hora del sistema
		time_t t;
		time(&t);
		struct tm *hora;
		hora = localtime(&t);
		
		// Para cada usuario
		for (int i = 0; i < usuarios; i++) {
			sprintf(ruta, "/proc/%d/fd/0", login[i].pid);
			if ((tty = fopen(ruta, "w")) == NULL) {
				perror("Error al abrir archivo");
				exit(EXIT_FAILURE);
			}

			fprintf(tty, "[%02d:%02d:%02d] Tu sesión terminará en %d segundos...\n", hora->tm_hour, hora->tm_min, hora->tm_sec, tiempo);

			fclose(tty);
		}

		tiempo -= INTERVALO;
		sleep(INTERVALO);
	}

	// Cerrar sesión para todos los usuarios
	for (int i = 0; i < usuarios; i++)
		kill(login[i].pid, 9);

	printf("Todas las sesiones cerradas.\n");

	exit(EXIT_SUCCESS);
}
