UNAME_S := $(shell uname -s)
CC=gcc
CFLAGS=-Wall -Wfatal-errors -Wextra -pedantic -Wmissing-declarations -std=c11
ASANFLAGS=-fsanitize=address -fno-common -fno-omit-frame-pointer
INCS=-I./libs/
LIBS=
LFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua
SRC=./src/*.c
BIN=tyler

# Mac Stuff
ifeq ($(UNAME_S),Darwin)
	INCS+= -I/opt/homebrew/include -I/opt/homebrew/Cellar/sdl2/2.26.3/include/SDL2/
	LIBS+= -L/opt/homebrew/lib -L/opt/homebrew/lib/
endif

all: clean build run

build:
	$(CC) $(SRC) $(CFLAGS) $(INCS) $(LIBS) $(LFLAGS) -o $(BIN)

debug:
	@$(CC) -g $(SRC) $(CFLAGS) $(INCS) $(LIBS) $(LFLAGS) -o debug

memcheck:
	@$(CC) -g $(SRC) $(ASANFLAGS) $(CFLAGS) $(INCS) $(LIBS) $(LFLAGS) -o memcheck.out
	@./memcheck.out
	@echo "Memory check passed"

run:
	./$(BIN)

clean:
	rm -rf $(BIN) debug*
