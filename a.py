import sys
from team import solve_maze


if __name__ == "__main__":
    
    path = f'./inputs/{sys.argv[1]}'
    with open(path, "r", encoding='utf-8') as f:
        maze = f.read()
    
    ms = solve_maze(maze)
    print(f"Tempo: {ms:.3f} ms\n")
    #print("Solução:\n", solve_maze.last_solution)