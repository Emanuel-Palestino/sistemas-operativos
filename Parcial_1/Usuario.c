/*
 * 
 * Programa que crea 3 procesos hijos que hacen lo siguiente:
 * Proceso 1: Busca y muestra toda la información del usuario (dentro del archivo passwd) que se ingresó en la línea de comandos
 * Proceso 2: Manda un correo al usuario
 * Proceso 3: Crea un directorio nuevo dentro del directorio en Home del usuario
 *
 * CCBY: Palestino Hernández Emanuel
 * Licencia: Apache 2.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>

// Función para encontrar coincidencia de un hijo en un arreglo.
// Como parámetros recibe el hijo, el apuntador al arreglo de hijos y el tamaño del arreglo
// Devuelve el indice del arreglo donde se encuentra el hijo o -1 si no lo encontró
int esHijo(pid_t, pid_t *, int);

int main(int argC, char *argV[]) {
    if (argC < 2) {
        printf("Ejcución incorrecta. Uso: ./usuario <nombreUsuario>\n");
        exit(EXIT_FAILURE);
    }

    char *nombreUsuario = argV[1];
    struct passwd *informacionUsuario;
    pid_t hijos[3];
    int estado;
    char nuevoDirectorio[100], correo[150];

    if ((informacionUsuario = getpwnam(nombreUsuario)) == NULL) {
        perror("Usuario no encontrado :c");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < 3; i++) {
        if ((hijos[i] = fork()) == -1) {
            perror("Error al crear el hijo");
            exit(EXIT_FAILURE);
        }

        if (hijos[i] == 0) {
            switch(i) {
                case 0:
                    printf("Informacion del archivo passwd:\n\t%s:%s:%d:%d:%s:%s:%s\n\n", informacionUsuario->pw_name, informacionUsuario->pw_passwd, informacionUsuario->pw_uid, informacionUsuario->pw_gid, informacionUsuario->pw_gecos, informacionUsuario->pw_dir, informacionUsuario->pw_shell);
                    break;
                case 1:
                    sprintf(correo, "echo \"Tienes tarea asignada para entregar el día del parcial, acuerdateee!!!\" | sendmail %s@emanuel.com", informacionUsuario->pw_name);
                    system(correo);
                    printf("Correo Eviado!\n\n");
                    break;
                case 2:
                    sprintf(nuevoDirectorio, "%s/tareajeje", informacionUsuario->pw_dir);
                    mkdir(nuevoDirectorio, 0777);
                    printf("Directorio Nuevo Creado!!!\n\n");
                    break;
            }

            exit(EXIT_SUCCESS);
        }
    }

    // Esperar a la finalización de los hijos
    for (int i = 0; i < 3; i++) {
       int posicionHijo = esHijo(wait(&estado), hijos, 3);
       if (posicionHijo == -1)
           perror("Hijo terminado anormalmente");
    }

    exit(EXIT_SUCCESS);
}

int esHijo(pid_t hijo, pid_t *hijos, int tamaño) {
    for (int i = 0; i < tamaño; i++) {
        if (hijo == hijos[i])
            return i;
    }

    return -1;
}
