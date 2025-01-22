#include "JoystickPantalla.h"

int running_program = 1;

//Funcion principal
int main(int argc, char **argv)
{
    if(argc <= 1)
    {
        printf("NO se ingreso el puerto por parametro\n");
        exit(1);
    }
    
    //Primer paso, definir variables
    int fd,fd2,puerto;
    socklen_t longitud_cliente;
    puerto = atoi(argv[1]);
    
    if (puerto < 1024)
    {
        printf("Puerto no válido\n");
        exit(1);
    }

    if (pthread_mutex_init(&mutex_running, NULL) != 0) {
        printf("Error al inicializar el mutex\n");
        return 1;
    }

    if (pthread_mutex_init(&mutex_point, NULL) != 0) {
        printf("Error al inicializar el mutex\n");
        return 1;
    }
    
    //Se necesitan dos estructuras del tipo sockaddr
    //La primera guarda info del server
    //La segunda del cliente
    struct sockaddr_in server;
    struct sockaddr_in client;
    
    //Configuracion del servidor
    server.sin_family= AF_INET; //Familia TCP/IP
    server.sin_port = htons(puerto); //Puerto
    server.sin_addr.s_addr = INADDR_ANY; //Cualquier cliente puede conectarse
    bzero(&(server.sin_zero),8); //Funcion que rellena con 0's
    
    //Paso 2, definicion de socket
    if ((fd = socket(AF_INET,SOCK_STREAM,0) ) < 0)
    {
        perror("Error de apertura de socket");
        exit(-1);
    }
    
    //Paso 3, avisar al sistema que se creo un socket
    if(bind(fd,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1) 
    {
        printf("error en bind() \n");
        exit(-1);
    }
    
    //Paso 4, establecer el socket en modo escucha
    if(listen(fd,5) == -1) 
    {
        printf("error en listen()\n");
        exit(-1);
    }

    int conexiones = 0;

    //Paso5, aceptar conexiones
    while(1 && running_program) 
    {
        
        longitud_cliente= sizeof(struct sockaddr_in);
        /* A continuacion la llamada a accept() */
        if ((fd2 = accept(fd,(struct sockaddr *)&client,&longitud_cliente))==-1) 
        {
            printf("error en accept()\n");
            exit(-1);
        }

        

        // Crear hilo para ejecutar la ventana de joystick
        if (conexiones >= MAX_CONEXIONES) {
            printf("Se ha alcanzado el máximo de conexiones\n");
            send(fd2,"No se ha podido establecer su conexión.\n",BUFFER_SIZE,0);
            close(fd2);
            continue;
        } else {
            send(fd2,"Bienvenido a emulador de joystick.\n",BUFFER_SIZE,0);

            //Obtener nombre del cliente
            char buffer[BUFFER_SIZE];
            send(fd2,"Digame su nombre:\n",23,0);
            recv(fd2,buffer,BUFFER_SIZE,0);
            send(fd2,"Gracias por conectarse\n",24,0);
            printf("Cliente conectado: %s",buffer);

            pid_t pid = fork();
            conexiones++;

            if (pid < 0) {
                // Error al crear el proceso
                perror("Error al crear el proceso");
                return -1;
            } else if (pid == 0) {
                // Código del proceso hijo
                buffer[strlen(buffer) - 1] = '\0'; // Eliminar el salto de línea
                pantalla(fd2, buffer);

                return 0; // Finalización ejecución
            }

            close(fd2); /* cierra fd2 */
        }
    }
    close(fd);

    pthread_mutex_destroy(&mutex_running);
    pthread_mutex_destroy(&mutex_point);
    return 0;
}

int pantalla (int fd2, char *nombre) {
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("No se pudo inicializar SDL: %s\\n", SDL_GetError());
        return 1;
    }

    // Crear la ventana
    SDL_Window *window = SDL_CreateWindow(
        nombre,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        printf("No se pudo crear la ventana: %s\\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Crear el renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("No se pudo crear el renderer: %s\\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Variables para la posición del punto
    int pointX = Punto_Inicial_X;
    int pointY = Punto_Inicial_Y;

    // Bucle principal
    int running = 1;
    SDL_Event event;

    // Crear hilos
    pthread_t hilo;

    // Datos para el hilo de lectura.
    struct ThreadData data;
    data.fd = fd2; // Ejemplo de valor
    data.pointX = &pointX;
    data.pointY = &pointY;
    data.running = &running;
    
    // Crear un hilo para leer datos
    if (pthread_create(&hilo, NULL, leer_dato, (void *)&data) != 0) {
        perror("Error al crear el hilo");
        return 1;
    }

    while (*data.running) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
                data.running = 0;
            }
        }

        // Limpiar la pantalla
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Negro
        SDL_RenderClear(renderer);
        
        // Dibujar el punto
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanco
        
        pthread_mutex_lock(&mutex_point);
        SDL_Rect pointRect = {*data.pointX, *data.pointY, POINT_SIZE, POINT_SIZE};
        pthread_mutex_unlock(&mutex_point);
        
        SDL_RenderFillRect(renderer, &pointRect);
        // Actualizar la pantalla
        SDL_RenderPresent(renderer);
        // Retraso para limitar la velocidad de actualización
        SDL_Delay(16); // ~60 FPS
    }

    printf("Cliente desconectado: %s\n", nombre);
    
    // Limpiar recursos
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
    
}

void* leer_dato(void* arg) {
    struct ThreadData* data = (struct ThreadData *)arg;

    char buffer_char[BUFFER_SIZE];
    memset(buffer_char, '\0', sizeof(buffer_char));

    int running = *data->running;

    while(running){

        if (recv(data->fd, buffer_char, BUFFER_SIZE, 0) == -1){
            printf("Error al leer datos\n");
        }

        char valor = buffer_char[0];
        pthread_mutex_lock(&mutex_point);
        switch (valor) {
            case 'U':
                if (*data->pointY > 0)
                    *data->pointY -= POINT_SPEED;
                break;
            case 'D':
                if (*data->pointY < SCREEN_HEIGHT - POINT_SIZE)
                    *data->pointY += POINT_SPEED;
                break;
            case 'L':
                if (*data->pointX > 0)
                    *data->pointX -= POINT_SPEED;
                break;
            case 'R':
                if (*data->pointX < SCREEN_WIDTH - POINT_SIZE)
                    *data->pointX += POINT_SPEED;
                break;
            case 'Q':
                *data->running = 0;
                break;
            case '+':
                *data->pointX = Punto_Inicial_X;
                *data->pointY = Punto_Inicial_Y;
                break;
        }
        
        pthread_mutex_unlock(&mutex_point);

        pthread_mutex_lock(&mutex_running);
        running = *data->running;
        pthread_mutex_unlock(&mutex_running);
    }


    return NULL;
}