import subprocess
import time


def solve_maze(labyrinth: str) -> float:
    # Compila o labirinto.c
    compile = subprocess.run(["gcc", "labirinto.c", "-o", "labirinto", "-O2"])
    if compile.returncode != 0:
        raise RuntimeError("Erro ao compilar labirinto.c")

    # Executa o programa compilado
    start = time.time()
    execute = subprocess.run(["./labirinto"])
    end = time.time()

    if execute.returncode != 0:
        raise RuntimeError("Erro ao executar ./labirinto")

    # Retorna o tempo em milissegundos
    return (end - start) * 1000


if __name__ == "__main__":
    tempo = solve_maze("nao importa o conteudo")
