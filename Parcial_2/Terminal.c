#include <stdio.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utmp.h>

typedef struct sesion {
	int pid;
	char tty[UT_LINESIZE];
} sesion;

int main() {
	struct utmp *usuario;
	int usuarios = 0;

	// Obtener cantidad de usuarios
	setutent();
	while(usuario = getutent()) {
		if (usuario->ut_type == USER_PROCESS && usuario->ut_line[0] == 't' && usuario->ut_line[0] == 't')
			usuarios++;
	}

	// Obtener información de usuarios
	sesion login[usuarios];
	int i = 0;
	setutent();
	while(usuario = getutent()) {
		if (usuario->ut_type == USER_PROCESS && usuario->ut_line[0] == 't' && usuario->ut_line[0] == 't') {
			login[i].pid = usuario->ut_pid;
			sprintf(login[i].tty, "%s", usuario->ut_line);
			i++;
		}
	}

	// Enviar mensajes usuarios
	FILE *tty;
	char ruta[100];
	sprintf(ruta, "/proc/%d/fd/0", login[0].pid);
	if ((tty = fopen(ruta, "w")) == NULL) {
		perror("Error al abrir archivo");
		return 1;
	}
	fprintf(tty, "Tu sesión terminará pronto jeje");
	
	fclose(tty);

	return 0;
}
