#include "JoystickTeclado.h"



int main(int argc, char *argv[])
{
    
    if(argc <= 2)
    {
        printf("No se ingreso el ip y puerto por parametro\n");
        exit(1);
    }

    //Primer paso, definir variables
    char *ip;
    int fd, numbytes,puerto;
    char buf[BUFFER_SIZE];
    puerto = atoi(argv[2]);
    ip = argv[1];
    
    struct hostent *he;
    /* estructura que recibira¡ informacion sobre el nodo remoto */
    struct sockaddr_in server;
    /* informacion sobre la direccion del servidor */
    
    if ((he=gethostbyname(ip))==NULL)
    {
        /* llamada a gethostbyname() */
        printf("gethostbyname() error\n");
        exit(-1);
    }
    
    //Paso 2, definicion de socket
    if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
        /* llamada a socket() */
        printf("socket() error\n");
        exit(-1);
    }

    //Datos del servidor
    server.sin_family = AF_INET;
    server.sin_port = htons(puerto);
    server.sin_addr = *((struct in_addr *)he->h_addr);
    
    /*he->h_addr pasa la informaciÃ³n de ``*he'' a "h_addr" */
    bzero(&(server.sin_zero),8);
    
    //Paso 3, conectarnos al servidor
    if(connect(fd, (struct sockaddr *)&server,
    sizeof(struct sockaddr))==-1)
    {
        /* llamada a connect() */
        printf("connect() error\n");
        exit(-1);
    }
    
    if ((numbytes=recv(fd,buf,BUFFER_SIZE,0)) == -1)
    {
        /* llamada a recv() */
        printf("Error en recv() \n");
        exit(-1);
    }
    
    buf[numbytes]='\0';

    if (buf[0] == 'N') {    // No se ha conectado al servidor
        printf("Mensaje del Servidor: %s\n",buf);
    } else {
        printf("%s",buf);

        recv(fd,buf,BUFFER_SIZE,0);
        printf("%s",buf);

        memset(buf, '\0', sizeof(buf)); // Limpiar el buffer
        fgets(buf, BUFFER_SIZE, stdin); // Leer el nombre del cliente 

        send(fd,buf,BUFFER_SIZE,0); // Enviar el nombre del cliente
        recv(fd,buf,BUFFER_SIZE,0); // Recibir mensaje de bienvenida
        printf("%s",buf);

        pantalla(fd);
    }

    close(fd);

    return 0;
}

int pantalla (int fd){
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("No se pudo inicializar SDL: %s\\n", SDL_GetError());
        return 1;
    }

    // Crear la ventana
    SDL_Window *window = SDL_CreateWindow(
        "Teclado",
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

    // Bucle principal
    int running = 1;
    SDL_Event event;

    // Variables para el teclado y ratón
    int mouseButtonDown = 0;
    int mouseX, mouseY;
    int keyDown = 0;
    SDL_Keycode keyPressed;

    // Buffer para los mensajes
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', sizeof(buffer));

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                keyDown = 1;
                keyPressed = event.key.keysym.sym;
            } else if (event.type == SDL_KEYUP) {
                keyDown = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                mouseButtonDown = 1;
                mouseX = event.button.x;
                mouseY = event.button.y;
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                mouseButtonDown = 0;
            }
        }

        // Movimientos del punto por teclado
        if (keyDown) {
            switch (keyPressed) {
                case SDLK_UP:
                    buffer[0] = 'U'; // Arriba
                    send(fd, buffer, BUFFER_SIZE, 0);
                    break;
                case SDLK_DOWN:
                    buffer[0] = 'D'; // Abajo
                    send(fd, buffer, BUFFER_SIZE, 0);
                    break;
                case SDLK_r: // Tecla 'R', reset
                    buffer[0] = '+';
                    send(fd, buffer, BUFFER_SIZE, 0);
                    break;
            }

            switch (keyPressed) {
                case SDLK_LEFT:
                    buffer[0] = 'L'; // Izquierda
                    send(fd, buffer, BUFFER_SIZE, 0);
                    break;
                case SDLK_RIGHT:
                    buffer[0] = 'R'; // Derecha
                    send(fd, buffer, BUFFER_SIZE, 0);
                    break;
                case SDLK_r: // Tecla 'R'
                    buffer[0] = '+';
                    send(fd, buffer, BUFFER_SIZE, 0);
                    break;
            }
        }else if (mouseButtonDown) {
            if (mouseX >= Punto_Inicial_X - 95 && mouseX <= Punto_Inicial_X + 95 && mouseY >= PUnto_Inicial_Y_Teclado - 295 && mouseY <= PUnto_Inicial_Y_Teclado - 105) {
                // Clic en el cuadrado superior
                buffer[0] = 'U';
                send(fd, buffer, BUFFER_SIZE, 0);
            } else if (mouseX >= Punto_Inicial_X - 95 && mouseX <= Punto_Inicial_X + 95 && mouseY >= PUnto_Inicial_Y_Teclado - 95 && mouseY <= PUnto_Inicial_Y_Teclado + 95) {
                // Clic en el cuadrado inferior
                buffer[0] = 'D';
                send(fd, buffer, BUFFER_SIZE, 0);
            } else if (mouseX >= Punto_Inicial_X - 295 && mouseX <= Punto_Inicial_X - 105 && mouseY >= PUnto_Inicial_Y_Teclado - 95 && mouseY <= PUnto_Inicial_Y_Teclado + 95) {
                // Clic en el cuadrado izquierdo
                buffer[0] = 'L';
                send(fd, buffer, BUFFER_SIZE, 0);
            } else if (mouseX >= Punto_Inicial_X + 105 && mouseX <= Punto_Inicial_X + 295 && mouseY >= PUnto_Inicial_Y_Teclado - 95 && mouseY <= PUnto_Inicial_Y_Teclado + 95) {
                // Clic en el cuadrado derecho
                buffer[0] = 'R';
                send(fd, buffer, BUFFER_SIZE, 0);
            } else if (mouseX >= Punto_Inicial_X + 200 && mouseX <= Punto_Inicial_X + 300 && mouseY >= PUnto_Inicial_Y_Teclado - 300 && mouseY <= PUnto_Inicial_Y_Teclado - 200) {
                // Clic en el cuadrado de reset
                buffer[0] = '+';
                send(fd, buffer, BUFFER_SIZE, 0);
            }
        }

        // Limpiar la pantalla
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Negro
        SDL_RenderClear(renderer);

        // Dibujar los cuadrados con flechas
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanco

        // Cuadrado y flecha arriba
        SDL_Rect squareUp = {Punto_Inicial_X - 95, PUnto_Inicial_Y_Teclado - 295, 190, 190};
        SDL_RenderDrawRect(renderer, &squareUp);
        SDL_RenderDrawLine(renderer, Punto_Inicial_X, PUnto_Inicial_Y_Teclado - 300 + 10, Punto_Inicial_X, PUnto_Inicial_Y_Teclado - 300 + 190);
        // Triángulo en la punta de la flecha hacia arriba
        SDL_RenderDrawLine(renderer, Punto_Inicial_X, PUnto_Inicial_Y_Teclado - 300 + 10, Punto_Inicial_X - 25, PUnto_Inicial_Y_Teclado - 300 + 60);
        SDL_RenderDrawLine(renderer, Punto_Inicial_X, PUnto_Inicial_Y_Teclado - 300 + 10, Punto_Inicial_X + 25, PUnto_Inicial_Y_Teclado - 300 + 60);
        SDL_RenderDrawLine(renderer, Punto_Inicial_X - 25, PUnto_Inicial_Y_Teclado - 300 + 60, Punto_Inicial_X + 25, PUnto_Inicial_Y_Teclado - 300 + 60);

        // Cuadrado y flecha abajo
        SDL_Rect squareDown = {Punto_Inicial_X - 95, PUnto_Inicial_Y_Teclado - 95, 190, 190};
        SDL_RenderDrawRect(renderer, &squareDown);
        SDL_RenderDrawLine(renderer, Punto_Inicial_X, PUnto_Inicial_Y_Teclado - 100 + 10, Punto_Inicial_X, PUnto_Inicial_Y_Teclado - 100 + 190);
        // Triángulo en la punta de la flecha hacia abajo
        SDL_RenderDrawLine(renderer, Punto_Inicial_X, PUnto_Inicial_Y_Teclado - 100 + 190, Punto_Inicial_X - 25, PUnto_Inicial_Y_Teclado - 100 + 140);
        SDL_RenderDrawLine(renderer, Punto_Inicial_X, PUnto_Inicial_Y_Teclado - 100 + 190, Punto_Inicial_X + 25, PUnto_Inicial_Y_Teclado - 100 + 140);
        SDL_RenderDrawLine(renderer, Punto_Inicial_X - 25, PUnto_Inicial_Y_Teclado - 100 + 140, Punto_Inicial_X + 25, PUnto_Inicial_Y_Teclado - 100 + 140);

        // Cuadrado y flecha izquierda
        SDL_Rect squareLeft = {Punto_Inicial_X - 295, PUnto_Inicial_Y_Teclado - 95, 190, 190};
        SDL_RenderDrawRect(renderer, &squareLeft);
        SDL_RenderDrawLine(renderer, Punto_Inicial_X - 300 + 10, PUnto_Inicial_Y_Teclado, Punto_Inicial_X - 300 + 190, PUnto_Inicial_Y_Teclado);
        // Triángulo en la punta de la flecha hacia la izquierda
        SDL_RenderDrawLine(renderer, Punto_Inicial_X - 300 + 10, PUnto_Inicial_Y_Teclado, Punto_Inicial_X - 300 + 60, PUnto_Inicial_Y_Teclado - 25);
        SDL_RenderDrawLine(renderer, Punto_Inicial_X - 300 + 10, PUnto_Inicial_Y_Teclado, Punto_Inicial_X - 300 + 60, PUnto_Inicial_Y_Teclado + 25);
        SDL_RenderDrawLine(renderer, Punto_Inicial_X - 300 + 60, PUnto_Inicial_Y_Teclado - 25, Punto_Inicial_X - 300 + 60, PUnto_Inicial_Y_Teclado + 25);

        // Cuadrado y flecha derecha
        SDL_Rect squareRight = {Punto_Inicial_X + 105, PUnto_Inicial_Y_Teclado - 95, 190, 190};
        SDL_RenderDrawRect(renderer, &squareRight);
        SDL_RenderDrawLine(renderer, Punto_Inicial_X + 100 + 10, PUnto_Inicial_Y_Teclado, Punto_Inicial_X + 100 + 190, PUnto_Inicial_Y_Teclado);
        // Triángulo en la punta de la flecha hacia la derecha
        SDL_RenderDrawLine(renderer, Punto_Inicial_X + 100 + 190, PUnto_Inicial_Y_Teclado, Punto_Inicial_X + 100 + 140, PUnto_Inicial_Y_Teclado - 25);
        SDL_RenderDrawLine(renderer, Punto_Inicial_X + 100 + 190, PUnto_Inicial_Y_Teclado, Punto_Inicial_X + 100 + 140, PUnto_Inicial_Y_Teclado + 25);
        SDL_RenderDrawLine(renderer, Punto_Inicial_X + 100 + 140, PUnto_Inicial_Y_Teclado - 25, Punto_Inicial_X + 100 + 140, PUnto_Inicial_Y_Teclado + 25);

        // Cuadrado de reset
        SDL_Rect squareReset = {Punto_Inicial_X + 200, PUnto_Inicial_Y_Teclado - 300, 100, 100};
        SDL_RenderDrawRect(renderer, &squareReset);

        // Actualizar la pantalla
        SDL_RenderPresent(renderer);

        // Retraso para limitar la velocidad de actualización
        SDL_Delay(16); // ~60 FPS
    }
    buffer[0] = 'Q';
    send(fd, buffer, BUFFER_SIZE, 0);

    // Limpiar recursos
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}