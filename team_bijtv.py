import ctypes

from datetime import datetime
import json

lib = ctypes.CDLL("./liblabyrinth.so")
lib.solve_maze_from_string.argtypes = [ctypes.c_char_p]
lib.solve_maze_from_string.restype = ctypes.c_void_p

libc = ctypes.CDLL("libc.dylib")  # macOS
libc.free.argtypes = [ctypes.c_void_p]


def solve_maze(labyrinth: str) -> float:
    """
    Recebe um labirinto como string (com \n separando as linhas)
    e retorna o tempo total em milissegundos para resolvê-lo.
    Gera 'output.txt' com o labirinto resolvido e 'output.json' com o caminho.
    """

    start = datetime.now()
    ptr = lib.solve_maze_from_string(labyrinth.encode())
    end = datetime.now()

    result = ctypes.string_at(ptr).decode()
    libc.free(ptr)

    elapsed_ms = (end - start).total_seconds() * 1000  # ✅ correto
    print(f"Tempo: {elapsed_ms:.3f} ms")

    # Salvar em output.txt
    with open("output.txt", "w") as f:
        f.write(result)

    # Gerar lista de coordenadas do caminho com '.'
    caminho = []
    linhas = result.strip().split("\n")
    for i, linha in enumerate(linhas):
        for j, c in enumerate(linha):
            if c == ".":
                caminho.append({"x": i, "y": j})

    # Salvar em output.json
    with open("output.json", "w") as f:
        json.dump({"Path": caminho}, f, indent=2)

    return elapsed_ms
