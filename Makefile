CC = gcc
CFLAGS = -Wall -pthread `sdl2-config --cflags --libs`
LDFLAGS = `sdl2-config --libs`

# Archivos fuente
SRCS = JoystickPantalla.c JoystickTeclado.c
INCLUDES = JoyStick.h JoyStickPantalla.h JoystickTeclado.h
# Archivos objeto
OBJS = $(SRCS:.c=.o)

# Ejecutables
EXES = JoystickPantalla JoystickTeclado

all: $(EXES)

JoystickPantalla: JoystickPantalla.o
	$(CC) -o $@ $^ $(LDFLAGS)

JoystickTeclado: JoystickTeclado.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXES)

.PHONY: all clean
