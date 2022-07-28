EXEC=./exec
INCLUDE_DIR=./include
CC=gcc
CFLAGS=-I$(INCLUDE_DIR)

OBJ_DIR=./obj

_HEADERS = morse_interpreter.h morse_data.h
HEADERS = $(patsubst %,$(INCLUDE_DIR)/%,$(_HEADERS))

_OBJ = main.o morse_interpreter.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

$(OBJ_DIR)/%.o: src/%.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC)/battleship: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR)/*.o

cleanexec:
	rm -rf $(EXEC)/battleship

fresh: clean cleanexec