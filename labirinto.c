#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>

#define MAX 501
#define WALL '#'
#define FREE ' '
#define START 'S'
#define END 'E'

typedef struct Node {
    int x, y;
    int g, h, f;
    struct Node *parent;
} Node;

int rows, cols;
char maze[MAX][MAX];
int visited[MAX][MAX];

Node *startNode = NULL, *endNode = NULL;

Node *create_node(int x, int y, int g, int h, Node *parent) {
    Node *node = malloc(sizeof(Node));
    node->x = x;
    node->y = y;
    node->g = g;
    node->h = h;
    node->f = g + h;
    node->parent = parent;
    return node;
}

int heuristic(int x, int y) {
    return abs(endNode->x - x) + abs(endNode->y - y);
}

int is_valid(int x, int y) {
    return x >= 0 && y >= 0 && x < rows && y < cols &&
           maze[x][y] != WALL && !visited[x][y];
}

Node *openList[MAX * MAX];
int openSize = 0;

void add_to_open(Node *node) {
    for (int i = 0; i < openSize; i++) {
        if (openList[i]->x == node->x && openList[i]->y == node->y) {
            if (openList[i]->f <= node->f) {
                free(node);
                return;
            } else {
                openList[i]->g = node->g;
                openList[i]->h = node->h;
                openList[i]->f = node->f;
                openList[i]->parent = node->parent;
                free(node);
                return;
            }
        }
    }
    openList[openSize++] = node;
}

Node *get_lowest_f_node() {
    int minIndex = 0;
    for (int i = 1; i < openSize; i++) {
        if (openList[i]->f < openList[minIndex]->f)
            minIndex = i;
    }
    Node *node = openList[minIndex];
    openList[minIndex] = openList[--openSize];
    return node;
}

void read_maze(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }
    rows = 0;
    cols = 0;
    memset(visited, 0, sizeof(visited));
    openSize = 0;
    startNode = endNode = NULL;

    while (fgets(maze[rows], MAX, file)) {
        maze[rows][strcspn(maze[rows], "\n")] = '\0';
        int len = strlen(maze[rows]);
        if (len > cols)
            cols = len;
        for (int j = 0; j < len; j++) {
            if (maze[rows][j] == START)
                startNode = create_node(rows, j, 0, 0, NULL);
            else if (maze[rows][j] == END)
                endNode = create_node(rows, j, 0, 0, NULL);
        }
        rows++;
    }
    fclose(file);
}

void reconstruct_path(Node *node, const char *jsonFile) {
    Node *path[MAX * MAX];
    int count = 0;

    while (node->parent) {
        if (maze[node->x][node->y] == FREE)
            maze[node->x][node->y] = '.';
        path[count++] = node;
        node = node->parent;
    }

    FILE *json = fopen(jsonFile, "w");
    if (!json) {
        perror("Erro ao criar JSON");
        return;
    }

    fprintf(json, "{\n  \"caminho\": [\n");
    for (int i = count - 1; i >= 0; i--) {
        fprintf(json, "    {\"x\": %d, \"y\": %d}%s\n", path[i]->x, path[i]->y, (i > 0 ? "," : ""));
    }
    fprintf(json, "  ]\n}\n");
    fclose(json);
}

Node *a_star() {
    add_to_open(startNode);

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    while (openSize > 0) {
        Node *current = get_lowest_f_node();

        if (current->x == endNode->x && current->y == endNode->y) {
            return current; // Retorna o nó final
        }

        visited[current->x][current->y] = 1;

        for (int i = 0; i < 4; i++) {
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];

            if (is_valid(nx, ny)) {
                int g = current->g + 1;
                int h = heuristic(nx, ny);
                Node *neighbor = create_node(nx, ny, g, h, current);
                add_to_open(neighbor);
            }
        }
    }

    return NULL;
}

void save_maze_to_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Erro ao criar arquivo de saída");
        return;
    }
    for (int i = 0; i < rows; i++) {
        fprintf(file, "%s\n", maze[i]);
    }
    fclose(file);
}

void process_all_inputs() {
    DIR *dir = opendir("inputs");
    if (!dir) {
        perror("Erro ao abrir diretório inputs/");
        exit(1);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".txt")) {
            char inputPath[256], jsonPath[256], txtPath[256], baseName[100];

            // Ex: maze1.txt → maze1
            strncpy(baseName, entry->d_name, strlen(entry->d_name) - 4);
            baseName[strlen(entry->d_name) - 4] = '\0';

            sprintf(inputPath, "inputs/%s", entry->d_name);
            sprintf(jsonPath, "outputs/%s.json", baseName);
            sprintf(txtPath, "outputs/%s.txt", baseName);

            printf("\nProcessando: %s\n", inputPath);

            read_maze(inputPath);
            clock_t start_time = clock();
            Node *end = a_star();
            clock_t end_time = clock();

            if (end) {
                reconstruct_path(end, jsonPath);
                save_maze_to_file(txtPath);
            } else {
                printf("Caminho não encontrado.\n");
            }

            double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;
            printf("Tempo: %.6f segundos (%.2f ms)\n", elapsed, elapsed * 1000);
        }
    }

    closedir(dir);
}

int main() {
    process_all_inputs();
    return 0;
}
