/*
**************************************Hijos.c*****************************************
**                                                                                  **
**  Descipción: Este programa crea n hijos mediante fork (n recibida por linea      **
**  de comandos) cada hijo imprime el numero i de hijo que es y el padre            **
**  espera la terminación cada hijo e imprime el PID del hijo que ha terminado.     **
**                                                                                  **
**  CCBY: Cruz Villalba Roberto Carlos Y Palestino Hernández Emanuel                **
**  Grupo: 602-A        Materia: Sistemas Operativos                                **
**  UNIVERSIDAD TECNOLÓGICA DE LA MIXTECA                                           **
**  Licencia: Apache 2.0                                                                 **
**************************************************************************************
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argC, char *argV[]){
    
    int estado;

    if(argC > 1) {
        
        int n = strtol(argV[1], NULL, 10);
        pid_t hijo;

        for(int i = 0; i < n; i++) {
            if(hijo = fork())
                break;
            if(hijo == 0){
                printf("Hijo numero: %d\n",i+1);
            }
        }

        if(wait(&estado) == hijo) {
            printf("soy el papa de %ld, que acaba de terminar\n",(long)getpid());
        }
        
    } else {
        printf("Debe colocar argumentos\n");
    }

    return EXIT_SUCCESS;
}
