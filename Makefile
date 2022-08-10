EXEC=./exec
INCLUDE_DIR=./include
CC=gcc
CFLAGS=-I$(INCLUDE_DIR)
# LINKER=-lncurses

OBJ_DIR=./obj

_HEADERS = morse_interpreter.h morse_data.h battleship.h playertools.h utils.h abilities.h
HEADERS = $(patsubst %,$(INCLUDE_DIR)/%,$(_HEADERS))

_OBJ = main.o morse_interpreter.o battleship.o playertools.o utils.o abilities.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

$(OBJ_DIR)/%.o: src/%.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LINKER)

$(EXEC)/battleship: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LINKER)

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR)/*.o

cleanexec:
	rm -rf $(EXEC)/battleship

fresh: clean cleanexec