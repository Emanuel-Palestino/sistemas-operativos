/* Crea un para  de hilos
para realizar exclusión mutua +SDL
*****modo de compilar:  gcc -Wall Autos.c -o Autos  `sdl-config --cflags --libs` -lpthread  */

#include <SDL/SDL.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#define MLONG 100

SDL_Surface *screen;
SDL_Surface *fondo;
SDL_Surface *nave;
SDL_Rect destino;
pthread_mutex_t EM;

void pantalla();
void *funcion1 (void *valor);
void *funcion2 (void *valor);

int main (int argc, char *argv[]) {
   pthread_t hilo1,hilo2;
   pthread_attr_t attr;

 /* Inicializar la biblioteca SDL */
   if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      printf("No se pudo inicializar SDL: %s\n", SDL_GetError());
      exit(1);
   }
   fondo=SDL_LoadBMP("fondo.bmp");
   nave=SDL_LoadBMP("auto.bmp");  
   SDL_SetColorKey(nave, SDL_SRCCOLORKEY,SDL_MapRGB(nave->format, 0, 0, 0));
   screen = SDL_SetVideoMode(1280, 720, 16, SDL_HWSURFACE );
   if( screen == NULL ) {
      printf( "Error al entrar a modo grafico: %s\n", SDL_GetError() );
      SDL_Quit();
      return -1;
   }
   pantalla();
   SDL_Flip(screen);

   // Coloca atributo predeterminados o coloque NULL
   pthread_attr_init(&attr);

   // Crear hilos
   pthread_create(&hilo1, &attr, funcion1, NULL);
   pthread_create(&hilo2, &attr, funcion2, NULL);

   //Espera hilos
   pthread_join(hilo1, NULL);
   pthread_join(hilo2, NULL);

   SDL_Quit();
   return EXIT_SUCCESS;
 }

void pantalla() {
   /* Titulo de la ventana */
   SDL_WM_SetCaption( "NAVES", "NAVES" );
   destino.x=0;
   destino.y=0;
   SDL_BlitSurface(fondo, NULL, screen, &destino);
}

void *funcion1 (void *valor) {
   int j=0;
   while(1) {
      // Enciende Semáforo (INICIO Exclusión) para indicar que entrara a SC
      pthread_mutex_lock(&EM);
      // Inicio de Sección Crítica
      pantalla();
      destino.x = 0 + j++;
      destino.y = 0 - 160;
      //if(j==20)j=0;
      SDL_BlitSurface(nave, NULL, screen, &destino);
      /* Actualizamos la pantalla */
      SDL_Flip(screen);
      // Fin de Sección Crítica
      // Apaga Semáforo (FIN Exclusión) para indicar que sale de SC
      pthread_mutex_unlock(&EM);
   }

   pthread_exit(0);
}

void *funcion2 (void *valor) {
   int j=0;
   while(1) {
      // Enciende Semáforo (INICIO Exclusión) para indicar que entrara a SC
      pthread_mutex_lock(&EM);
      // Inicio de Sección Crítica
      pantalla();
      destino.x = 0 + j++;
      destino.y = 200 - 160;
      //if(j==20)j=0;
      SDL_BlitSurface(nave, NULL, screen, &destino);
      SDL_Flip(screen);
      // Fin de Sección Crítica
      // Apaga Semáforo (FIN Exclusión) para indicar que sale de SC
      pthread_mutex_unlock(&EM);
   }
   pthread_exit(0);
}
