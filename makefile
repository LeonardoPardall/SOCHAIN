# Grupo 53
# Leonardo Pardal 61836
# Martim Cardoso 61876
# Nuno Nobre 61823

BIN_DIR = bin
INC_DIR = inc
OBJ_DIR = obj
SRC_DIR = src
CC = gcc

OBJECTS_FILES = main.o memory.o process.o server.o wallet.o
OBJECTS = $(addprefix $(OBJ_DIR)/, $(OBJECTS_FILES))

all: $(OBJECTS)
	@$(CC) $(OBJECTS) -o $(BIN_DIR)/SOchain

clean:
	@rm -f $(OBJ_DIR)/*
	@rm -f $(BIN_DIR)/SOchain

re: clean all

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) -c $< -o $@ -I $(INC_DIR)