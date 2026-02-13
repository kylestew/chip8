CC      = cc
CFLAGS  = -Wall -Wextra -std=c99 $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)

SRC     = main.c chip8.c rom.c graphics.c
OBJ     = $(SRC:.c=.o)
BIN     = chip8

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
