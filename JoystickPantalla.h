#include "Joystick.h"

// Estructura para encapsular las variables
struct ThreadData {
    int fd;
    int *pointX;
    int *pointY;
    int *running; //Ver el evento para finalizar
};

pthread_mutex_t mutex_running;
pthread_mutex_t mutex_point;
pthread_mutex_t mutex_running = PTHREAD_MUTEX_INITIALIZER;

int pantalla (int fd2, char *nombre);
void* leer_dato(void* arg);