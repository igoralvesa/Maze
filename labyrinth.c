#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX 501
#define WALL '#'
#define FREE ' '
#define START 'S'
#define END 'E'

typedef struct Node
{
    int x, y;
    int g, h, f;
    struct Node *parent;
} Node;

int rows, cols;
char maze[MAX][MAX];
int visited[MAX][MAX];
Node *startNode = NULL, *endNode = NULL;
Node *openList[MAX * MAX];
int openSize = 0;

Node *create_node(int x, int y, int g, int h, Node *parent)
{
    Node *node = malloc(sizeof(Node));
    node->x = x;
    node->y = y;
    node->g = g;
    node->h = h;
    node->f = g + h;
    node->parent = parent;
    return node;
}

int heuristic(int x, int y)
{
    return abs(endNode->x - x) + abs(endNode->y - y);
}

int is_valid(int x, int y)
{
    return x >= 0 && y >= 0 && x < rows && y < cols &&
           maze[x][y] != WALL && !visited[x][y];
}

void add_to_open(Node *node)
{
    for (int i = 0; i < openSize; i++)
    {
        if (openList[i]->x == node->x && openList[i]->y == node->y)
        {
            if (openList[i]->f <= node->f)
            {
                free(node);
                return;
            }
            else
            {
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

Node *get_lowest_f_node()
{
    int minIndex = 0;
    for (int i = 1; i < openSize; i++)
    {
        if (openList[i]->f < openList[minIndex]->f)
            minIndex = i;
    }
    Node *node = openList[minIndex];
    openList[minIndex] = openList[--openSize];
    return node;
}

void reconstruct_path(Node *node)
{
    while (node->parent)
    {
        if (maze[node->x][node->y] == FREE)
            maze[node->x][node->y] = '.';
        node = node->parent;
    }
}

void a_star()
{
    add_to_open(startNode);

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    while (openSize > 0)
    {
        Node *current = get_lowest_f_node();

        if (current->x == endNode->x && current->y == endNode->y)
        {
            reconstruct_path(current);
            return;
        }

        visited[current->x][current->y] = 1;

        for (int i = 0; i < 4; i++)
        {
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];

            if (is_valid(nx, ny))
            {
                int g = current->g + 1;
                int h = heuristic(nx, ny);
                Node *neighbor = create_node(nx, ny, g, h, current);
                add_to_open(neighbor);
            }
        }
    }
}

char *solve_maze_from_string(const char *labstr)
{
    // Reset
    memset(visited, 0, sizeof(visited));
    openSize = 0;
    startNode = endNode = NULL;
    rows = cols = 0;

    // Lê string do labirinto linha por linha
    const char *p = labstr;
    while (*p && rows < MAX)
    {
        int len = 0;
        while (p[len] && p[len] != '\n')
            len++;

        strncpy(maze[rows], p, len);
        maze[rows][len] = '\0';

        if (len > cols)
            cols = len;

        for (int j = 0; j < len; j++)
        {
            if (maze[rows][j] == START)
                startNode = create_node(rows, j, 0, 0, NULL);
            else if (maze[rows][j] == END)
                endNode = create_node(rows, j, 0, 0, NULL);
        }

        rows++;
        p += len + (p[len] == '\n' ? 1 : 0);
    }

    if (!startNode || !endNode)
        return NULL;

    a_star();

    // Constrói a string de retorno que será recebida pelo arquivo python
    size_t total = (rows * (cols + 1)) + 1;
    char *out = malloc(total);
    char *q = out;

    for (int i = 0; i < rows; i++)
    {
        int len = strlen(maze[i]);
        memcpy(q, maze[i], len);
        q += len;
        *q++ = '\n';
    }

    *q = '\0';
    return out;
}
