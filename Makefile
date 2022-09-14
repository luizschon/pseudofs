# Variáveis de compilação
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -O2
LIBS = -pthread

# Nome dos diretórios
SRC = src
OBJ = obj

# Nome dos arquivos fonte, objeto e executável
SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
BIN = pseudofs

# Nome do arquivo zip e arquivos a serem ignorados
ZNAME = Trabalho2_SO_2022-1.zip
ZIGNORE = ./$(OBJ)/\* $(BIN)

all: release

release: CFLAGS += -DNDEBUG
release: $(BIN)

debug: CFLAGS += -g -fno-omit-frame-pointer
debug: LIBS += -fsanitize=address
debug: $(BIN)

clean: 
	rm -rf $(BIN) $(OBJ)
	@echo "Limpeza realizada"

$(BIN): $(OBJS)
	@$(CC) $(LIBS) $(OBJS) -o $@
	@echo "Build realizada com sucesso!"

$(OBJS): $(OBJ)/%.o: $(SRC)/%.c
	@echo Compilando $<
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c -o $@ $<

zip:
	@rm -f $(ZNAME)
	@zip -r $(ZNAME) . -x $(ZIGNORE)
	@echo "Arquivo ZIP gerado com sucesso!"

.PHONY: release debug clean

