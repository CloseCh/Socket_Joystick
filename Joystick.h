#include <stdio.h>
#include <SDL2/SDL.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> //netbd.h es necesitada por la estructura hostent
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

// Dimensiones de la ventana
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define POINT_SPEED 10
#define POINT_SIZE  10

#define Punto_Inicial_X (SCREEN_WIDTH / 2)
#define Punto_Inicial_Y (SCREEN_HEIGHT / 2)

#define PUnto_Inicial_Y_Teclado (Punto_Inicial_Y + 50)

// Tamaño del buffer de escritura y lectura
#define BUFFER_SIZE 100


// Maximo de conexiones soportadas
#define MAX_CONEXIONES 3