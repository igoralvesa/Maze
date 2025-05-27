import subprocess
import time
import os


def solve_maze(labyrinth: str) -> float:
    # Compila o labirinto.c
    # compile = subprocess.run(["gcc", "labirinto.c", "-o", "labirinto", "-O2"])
 

    # Executa o programa compilado
    start = time.time()
    #execute = subprocess.run(["./labirinto"])
    os.system('./labirinto')
    end = time.time()

    

    # Retorna o tempo em milissegundos
    teste = (end - start) * 1000
    print(teste)
    return teste


if __name__ == "__main__":
    tempo = solve_maze("nao importa o conteudo")
