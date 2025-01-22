#include "Joystick.h"

struct ThreadData {
    int fd;
    int *running; //Ver el evento para finalizar
};

pthread_mutex_t mutex_running = PTHREAD_MUTEX_INITIALIZER;

int pantalla (int fd);