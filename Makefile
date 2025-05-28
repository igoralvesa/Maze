# Compilador e flags padrão
CC = gcc
CFLAGS = -O2 -Wall -fPIC

# Arquivos gerados
TARGET = labyrinth
LIBSO = liblabyrinth.so 

# Alvo padrão: compila e executa standalone
all: run

# Compila e executa o programa principal
run: $(TARGET)
	./$(TARGET)

# Compila o binário standalone
$(TARGET): labyrinth.c
	$(CC) $(CFLAGS) -o $(TARGET) labyrinth.c

# Compila como biblioteca compartilhada (.so) para uso com ctypes no Python
lib: labyrinth.c
	$(CC) $(CFLAGS) -shared -o $(LIBSO) labyrinth.c

# Limpa arquivos gerados (binário, .so, saída e cache Python)
clean:
	@rm -f output.json
	@rm -f output.txt
	@rm -f $(TARGET)
	@rm -f $(LIBSO)
	@rm -rf __pycache__/
	@rm -f *.pyc
