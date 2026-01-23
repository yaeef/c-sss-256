# Compilador y flags
CC      = gcc
CFLAGS  = -Wall -Iinclude
LIBS    = -lcrypto

# Directorios
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Archivos fuente
COMMON_SRC = sss.c field.c sssio.c

# Archivos objeto comunes
COMMON_OBJ = $(COMMON_SRC:%.c=$(OBJ_DIR)/%.o)

# Targets principales
ALL_BINS = $(BIN_DIR)/sss $(BIN_DIR)/test $(BIN_DIR)/gen

all: $(ALL_BINS)

# ===== Binarios =====

$(BIN_DIR)/sss: $(OBJ_DIR)/main.o $(COMMON_OBJ)
	$(CC) $^ -o $@ $(LIBS)

$(BIN_DIR)/test: $(OBJ_DIR)/test.o $(COMMON_OBJ)
	$(CC) $^ -o $@ $(LIBS)

$(BIN_DIR)/gen: $(OBJ_DIR)/generador.o
	$(CC) $^ -o $@ $(LIBS)

# ===== Compilación a .o =====

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ===== Utilidades =====

clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/*

dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean dirs

