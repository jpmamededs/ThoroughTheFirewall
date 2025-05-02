TARGET = JogoAED

SRC_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib_raylib
RELEASE_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Compilador e opções
CC = gcc
CFLAGS = -Wall -std=c99 -I$(INCLUDE_DIR) -Icurl/include -mwindows
LIBS = -L$(LIB_DIR) -Lcurl/lib -lraylib -lcurl -lopengl32 -lgdi32 -lwinmm
EXE_EXT = .exe

BIN_TARGET = $(RELEASE_DIR)/$(TARGET)$(EXE_EXT)

# Compilar para pasta bin
$(BIN_TARGET): $(SOURCES)
	@mkdir -p $(RELEASE_DIR)
	$(CC) $(CFLAGS) $(SOURCES) -o $@ $(LIBS)

# Executar o jogo
run: $(BIN_TARGET)
	./$<

# Compilar e rodar em sequência
makerun: $(BIN_TARGET)
	./$<

# Limpar binário
clean:
	cmd /C "del /Q $(RELEASE_DIR)\$(TARGET)$(EXE_EXT) 2>nul || echo Nada para limpar"
