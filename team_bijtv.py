import ctypes

from datetime import datetime
import json
import platform

# Comando para compilar o C:
# gcc -O2 -fPIC -shared -o liblabyrinth.so labyrinth.c

lib = ctypes.CDLL("./liblabyrinth.so")
lib.solve_maze_from_string.argtypes = [ctypes.c_char_p]
lib.solve_maze_from_string.restype = ctypes.c_void_p

if platform.system() == "Darwin": # macOS
    libc = ctypes.CDLL("libc.dylib")
else:                             # Linux
    libc = ctypes.CDLL("libc.so.6") 
    
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

    elapsed_ms = (end - start).total_seconds() * 1000
    print(f"Tempo: {elapsed_ms:.3f} ms")

    # output.txt
    with open("output.txt", "w") as f:
        f.write(result)

    # output.json
    caminho = []
    linhas = result.strip().split("\n")
    for i, linha in enumerate(linhas):
        for j, c in enumerate(linha):
            if c == ".":
                caminho.append({"x": i, "y": j})

    with open("output.json", "w") as f:
        json.dump({"Path": caminho}, f, indent=2)

    return elapsed_ms
