# Alvo padrão (executado se chamar só "make")
all: run

# Compila e executa o programa
run:
	gcc labirinto.c -o labirinto
	./labirinto

# Remove arquivos de saída
clean:
	@mkdir -p outputs
	rm -f outputs/*.json
	rm -f outputs/*.txt
	rm -f labirinto