EXEC=./exec
INCLUDE_DIR=./include
CC=gcc
CFLAGS=-I$(INCLUDE_DIR)
# LINKER=-lncurses

OBJ_DIR=./obj

ifeq ($(GUI), 1)
	GTK_CFLAGS=pkg-config --cflags --libs gtk+-3.0
	_HEADERS = battleship.h playertools.h utils.h abilities.h dedicatedGUI.h
	_OBJ = main.o battleship.o playertools.o utils.o abilities.o dedicatedGUI.o
	CFLAGS += -DGUI_ENABLE
else
	_HEADERS = battleship.h playertools.h utils.h abilities.h
	_OBJ = main.o battleship.o playertools.o utils.o abilities.o
endif

HEADERS = $(patsubst %,$(INCLUDE_DIR)/%,$(_HEADERS))
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

$(OBJ_DIR)/%.o: src/%.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) `$(GTK_CFLAGS)` $(LINKER) 

$(EXEC)/battleship: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) `$(GTK_CFLAGS)` $(LINKER)

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR)/*.o

cleanexec:
	rm -rf $(EXEC)/battleship

fresh: clean cleanexec