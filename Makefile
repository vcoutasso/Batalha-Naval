PROJ_NAME=Batalha-Naval

# .c files
SOURCE=$(wildcard ./src/*.c)

# .h files
HEADERS=$(wildcard ./src/*.h)

# Object files
OBJ=$(subst .c,.o,$(subst src,obj,$(SOURCE)))

# Compiler
CC=gcc

# Compiler flags
CC_FLAGS =	-c		\
			-Wall	\
			-pedantic

# Linker flags
L_FLAGS = `pkg-config --libs allegro-5 allegro_audio-5 allegro_dialog-5 allegro_image-5 allegro_primitives-5 allegro_acodec-5 allegro_color-5 allegro_font-5 allegro_main-5 allegro_ttf-5`

# Compile and link
all: objFolder $(PROJ_NAME)

$(PROJ_NAME): $(OBJ)
	@ echo -e '\033[1;37mBuilding binary using GCC Linker:\033[0m $@'
	$(CC) -o $@ $^ $(L_FLAGS)
	@ echo
	@ echo -e '\033[1;37mFinished building binary:\033[0m $@'

./obj/%.o: ./src/%.c ./src/%.h
	@ echo -e '\033[1;37mBuilding target using GCC compiler:\033[0m $<'
	$(CC) -o $@ $< $(CC_FLAGS)
	@ echo

./obj/main.o: ./src/main.c $(HEADERS)
	@ echo -e '\033[1;37mBuilding target using GCC compiler:\033[0m $<'
	$(CC) -o $@ $< $(CC_FLAGS)
	@ echo

debug: L_FLAGS += -DDEBUG -g
debug: CC_FLAGS += -DDEBUG -g
debug: objFolder $(PROJ_NAME)

objFolder:
	@ mkdir -p obj

remake: clean objFolder $(PROJ_NAME)

clean:
	@ echo -e '\033[1;37mCleaning up files...\033[0m'
	@ rm -rf ./obj *~ $(PROJ_NAME)
	@ rm -f tabuleiro*.txt recordes.txt
