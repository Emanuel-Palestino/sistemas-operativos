/*
 * 
 * Ejecuta la una función igual al comando "ls -l" a nivel de impresión, 
 * sin embargo al encontrar una carpeta, imprime el contenido de la misma de manera recursiva y así
 * sucesivamente para todas las carpetas encontradas.
 * CCBY: Cruz Villalba Roberto Carlos, Palestino Hernández Emanuel
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
 #include <pwd.h> 
 #include <grp.h>  

#define RUTA 255

//funcion calcularTotal(char* ruta_a_recorrer) recorre un directorio sumando el total de bloques que ocupa y los devuelve en un entero.
int calcularTotal(char*);
/*Función recorrerCarpeta(char* ruta_a_recorrer, int taboladores) recorre el directorio que recibe imprimiendo la información de cada archivo en él, 
si encuentra una carpeta se llama a sí misma. La cantidad de tabuladores aumenta cada vez que se llama a sí misma así proporcionando identado a la impresion*/
void recorrerCarpeta(char*,int);

/*Fuente: https://programmerclick.com/article/3464812837/*/

/*mode_to_letters(int modo, char ----------) el modo es el modo que se obtiene del stat del archivo que manda a llamar a la función. La cadena son la cadena
en la que se incrustarán las letras para ser visualizadas de la forma que lo entrega ls -la. Ejemplo de un archivo de tipo directorio con permisos de
lectura-escritura-ejecucion,lectura-escritura-ejecucion,lecutra,-,ejecución. Imprime: drwxrwxr-x */

void mode_to_letters(int mode, char str[]);
/*show_file_info(char* nombre_archivo_x, struct stat* stat_del_archivo_x)
impmime la informacion del archivo_x a partir del stat recibido en el segundo parámetro*/
void show_file_info(char* filename, struct stat* info_p); 

/*char* gid_to_name(git_t identificador) 
convierte el identificador recibido y le extrae el nombre al grupo para devolverlo como una cadena*/
char* gid_to_name(gid_t gid);  
/*char* uid_to_name(git_t identificador) 
convierte el identificador recibido y le extrae el nombre al usuaario para devolverlo como una cadena*/
char* uid_to_name(uid_t uid); 


int main(void) {
    char ruta [RUTA + 1];

	if (getcwd(ruta, RUTA + 1) == NULL) {
		perror("No se tuvo acceso al directorio");
		exit(EXIT_FAILURE);
	}
	printf("Total %d\n",calcularTotal(ruta));
    recorrerCarpeta(ruta,0);

	exit(EXIT_SUCCESS);
    }


void recorrerCarpeta(char* ruta, int tabuladores){
    struct stat sb;
	printf("Ruta actual: %s\n", ruta);
	//printf("Total %d\n",calcularTotal(ruta));



	DIR *dir;
	struct dirent *direntrada;
	if ((dir = opendir(ruta)) == NULL) {
		perror("No puedo leer el directorio");
	}

	while((direntrada = readdir(dir)) != NULL) {
		char rutaAux[2*RUTA];
		sprintf(rutaAux,"%s/%s",ruta,direntrada->d_name);
        //obtener stat de archivo
		if((lstat(rutaAux, &sb))== -1){
			printf("Nombre del archivo: %s", direntrada->d_name);
			perror("Error en lstat");
			exit(EXIT_FAILURE);
		}
		
        //identificar si es una carpeta
        if(S_ISDIR(sb.st_mode) ){
			if(strlen(direntrada->d_name) >= 1 && direntrada->d_name[0] != '.'){
				if(strlen(direntrada->d_name) >= 2 && direntrada->d_name[0] != '.' && direntrada->d_name[1] != '.'){
					//printf("\n%s es una carpeta Válida\n",direntrada->d_name);		
            		recorrerCarpeta(rutaAux,tabuladores+1);
				}
			}
		}

		/*impresión de formato ls*/
		for(int i=0; i<tabuladores ; i++)
			printf("\t");

		//printf("Nombre del Archivo: %s\n", direntrada->d_name);
        show_file_info(direntrada->d_name, &sb);


	}
	closedir(dir);
}
int calcularTotal(char* ruta){
	
    
	struct stat sb;
	struct dirent *direntrada;
	DIR *dir;
	int total = 0;

	if ((dir = opendir(ruta)) == NULL) {
		perror("No puedo leer el directorio");
	}
	while((direntrada = readdir(dir)) != NULL){
		if(lstat(direntrada->d_name,&sb)!=0){
			perror("No se pudo obtener la información del archivo\n");
			exit(EXIT_FAILURE);
		}

	
        total += sb.st_blocks;


	}
	total *= 500;
    total /= 1000;	
	return total;
}


void show_file_info(char* filename, struct stat* info_p)  
    {  
        char* uid_to_name(), *ctime(), *gid_to_name(), *filemode();  
        void mode_to_letters();  
        char modestr[11];  
        mode_to_letters(info_p->st_mode, modestr);  
      
        printf("%s", modestr);  
        printf(" %4d", (int) info_p->st_nlink);  
        printf(" %-8s", uid_to_name(info_p->st_uid));  
        printf(" %-8s", gid_to_name(info_p->st_gid));  
        printf(" %8ld", (long) info_p->st_size);  
        printf(" %.12s", 4 + ctime(&info_p->st_mtime));  
		if(S_ISDIR(info_p->st_mode)){
			//"\x1b[34m"
        	printf("\x1b[34m" " %s\n" "\x1b[0m", filename);  
		}else{
			printf(" %s\n", filename); 
		}
    } 


void mode_to_letters(int mode, char str[])  
    {  
        strcpy(str, "----------");  
      
        if (S_ISDIR(mode))  
        {  
            str[0] = 'd';  
        }  
      
        if (S_ISCHR(mode))  
        {  
            str[0] = 'c';  
        }  
      
        if (S_ISBLK(mode))  
        {  
            str[0] = 'b';  
        }  
      
        if ((mode & S_IRUSR))  
        {  
            str[1] = 'r';  
        }  
      
        if ((mode & S_IWUSR))  
        {  
            str[2] = 'w';  
        }  
      
        if ((mode & S_IXUSR))  
        {  
            str[3] = 'x';  
        }  
      
        if ((mode & S_IRGRP))  
        {  
            str[4] = 'r';  
        }  
      
        if ((mode & S_IWGRP))  
        {  
            str[5] = 'w';  
        }  
      
        if ((mode & S_IXGRP))  
        {  
            str[6] = 'x';  
        }  
      
        if ((mode & S_IROTH))  
        {  
            str[7] = 'r';  
        }  
      
        if ((mode & S_IWOTH))  
        {  
            str[8] = 'w';  
        }  
      
        if ((mode & S_IXOTH))  
        {  
            str[9] = 'x';  
        }  
    } 
    char* uid_to_name(uid_t uid)  
    {  
        struct passwd* getpwuid(),* pw_ptr;  
        static char numstr[10];  
      
        if((pw_ptr = getpwuid(uid)) == NULL)  
        {  
            sprintf(numstr,"%d",uid);  
      
            return numstr;  
        }  
        else  
        {  
            return pw_ptr->pw_name;  
        }  
    }  
      
    char* gid_to_name(gid_t gid)  
    {  
        struct group* getgrgid(),* grp_ptr;  
        static char numstr[10];  
      
        if(( grp_ptr = getgrgid(gid)) == NULL)  
        {  
            sprintf(numstr,"%d",gid);  
            return numstr;  
        }  
        else  
        {  
            return grp_ptr->gr_name;  
        }  
    } 