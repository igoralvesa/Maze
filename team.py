import os
import time
import sys
import ctypes
from ctypes import CDLL, c_char_p, c_void_p

os.system("gcc -fPIC -O2 -shared labirinto.c -o liblabirinto.so -lm")

# 2) carrega a lib
_lib = CDLL("./liblabirinto.so")

# 3) define argtypes e restype: retorno puro como ponteiro
_lib.solve_maze_from_string.argtypes = [c_char_p]
_lib.solve_maze_from_string.restype  = c_void_p

# 4) prepara free() da libc
_libc = ctypes.CDLL("libc.so.6")    # macOS: "libc.dylib"
_libc.free.argtypes = [c_void_p]
_libc.free.restype  = None

def solve_maze(labyrinth: str) -> float:
    """
    Recebe labirinto em `labyrinth` (\n-separated),
    chama a C, mede o tempo e retorna só o tempo em ms.
    A solução vem em solve_maze.last_solution.
    """
    t0 = time.time()
    ptr = _lib.solve_maze_from_string(labyrinth.encode('utf-8'))
    t1 = time.time()

    # lê a C-string (até '\0') e decodifica
    solved = ctypes.string_at(ptr).decode('utf-8')

    # libera o buffer alocado no C
    _libc.free(ptr)

    solve_maze.last_solution = solved
    return (t1 - t0) * 1000.0

if __name__ == "__main__":
    
    path = f'./inputs/{sys.argv[1]}'
    with open(path, "r", encoding='utf-8') as f:
        maze = f.read()
    
    ms = solve_maze(maze)
    print(f"Tempo: {ms:.3f} ms\n")
    #print("Solução:\n", solve_maze.last_solution)
