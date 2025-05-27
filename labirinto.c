// labirinto.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#define MAX   501
#define WALL  '#'
#define FREE  ' '
#define START 'S'
#define END   'E'

typedef struct Node {
    int x, y;           // posição no labirinto
    int g, h, f;        // custos g(n), h(n) e f = g + h
    struct Node *parent;
} Node;

// —————————————————————————————————————————————————————
//  Globals do A*
// —————————————————————————————————————————————————————
static int rows, cols;
static char maze[MAX][MAX];
static int visited[MAX][MAX];
static Node *startNode, *endNode;
static Node *openList[MAX * MAX];
static int openSize;

// —————————————————————————————————————————————————————
//  Funções auxiliares
// —————————————————————————————————————————————————————

// Cria um novo nó
static Node *create_node(int x, int y, int g, int h, Node *parent) {
    Node *n = malloc(sizeof(Node));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    n->x = x; n->y = y;
    n->g = g; n->h = h; n->f = g + h;
    n->parent = parent;
    return n;
}

// Heurística de Manhattan
static int heuristic(int x, int y) {
    return abs(endNode->x - x) + abs(endNode->y - y);
}

// Verifica se posição (x,y) é válida para explorar
static int is_valid(int x, int y) {
    return x >= 0 && y >= 0 && x < rows && y < cols
        && maze[x][y] != WALL
        && !visited[x][y];
}

// Adiciona nó à open list, mantendo o melhor f para cada posição
static void add_to_open(Node *node) {
    for (int i = 0; i < openSize; i++) {
        Node *o = openList[i];
        if (o->x == node->x && o->y == node->y) {
            if (o->f <= node->f) {
                free(node);
                return;
            } else {
                // atualiza
                o->g = node->g;
                o->h = node->h;
                o->f = node->f;
                o->parent = node->parent;
                free(node);
                return;
            }
        }
    }
    openList[openSize++] = node;
}

// Retira e retorna o nó de menor f da open list
static Node *get_lowest_f_node(void) {
    int best = 0;
    for (int i = 1; i < openSize; i++) {
        if (openList[i]->f < openList[best]->f)
            best = i;
    }
    Node *res = openList[best];
    openList[best] = openList[--openSize];
    return res;
}

// Reconstrói o caminho quando o fim é alcançado
static void reconstruct_path(Node *node) {
    while (node->parent) {
        if (maze[node->x][node->y] == FREE)
            maze[node->x][node->y] = '.';
        node = node->parent;
    }
}

// Implementação do A*
static void a_star(void) {
    // adiciona o ponto de início
    add_to_open(startNode);

    // offsets de vizinhança (N, S, W, E)
    const int dx[4] = { -1,  1,  0,  0 };
    const int dy[4] = {  0,  0, -1, +1 };

    while (openSize > 0) {
        Node *cur = get_lowest_f_node();
        if (cur->x == endNode->x && cur->y == endNode->y) {
            reconstruct_path(cur);
            return;
        }
        visited[cur->x][cur->y] = 1;

        // expande vizinhos
        for (int i = 0; i < 4; i++) {
            int nx = cur->x + dx[i];
            int ny = cur->y + dy[i];
            if (is_valid(nx, ny)) {
                int g = cur->g + 1;
                int h = heuristic(nx, ny);
                Node *nbr = create_node(nx, ny, g, h, cur);
                add_to_open(nbr);
            }
        }
    }
    // se esgotar sem encontrar caminho, nada faz — o labirinto fica inalterado
}

// —————————————————————————————————————————————————————
//  API exposta para ctypes
// —————————————————————————————————————————————————————

/**
 * Recebe o labirinto como única string com linhas separadas por '\n',
 * resolve o A* em memória (marca o caminho com '.') e devolve
 * uma nova string malloc’ed com o labirinto resolvido
 * (cada linha termina em '\n'). Caller deve free() o retorno.
 */
char *solve_maze_from_string(const char *labstr) {
    // 1) limpa estado
    memset(visited, 0, sizeof(visited));
    openSize = 0;
    startNode = endNode = NULL;
    rows = cols = 0;

    // 2) “leitura” linha a linha de labstr
    const char *p = labstr;
    while (*p && rows < MAX) {
        // conta comprimento até '\n' ou fim
        int len = 0;
        while (p[len] && p[len] != '\n') len++;
        // copia para maze
        memcpy(maze[rows], p, len);
        maze[rows][len] = '\0';
        // ajusta cols para a maior largura
        if (len > cols) cols = len;
        // procura START e END
        for (int j = 0; j < len; j++) {
            if (maze[rows][j] == START)
                startNode = create_node(rows, j, 0, 0, NULL);
            else if (maze[rows][j] == END)
                endNode   = create_node(rows, j, 0, 0, NULL);
        }
        rows++;
        // avança além de '\n'
        p += len + (p[len] == '\n' ? 1 : 0);
    }

    // 3) roda o A*
    a_star();

    // 4) constrói string de saída
    size_t total = (size_t)rows * (cols + 1) + 1;
    char *out = malloc(total);
    if (!out) return NULL;
    char *q = out;
    for (int i = 0; i < rows; i++) {
        int linelen = strlen(maze[i]);
        memcpy(q, maze[i], linelen);
        q += linelen;
        *q++ = '\n';
    }
    *q = '\0';

    return out;
}
