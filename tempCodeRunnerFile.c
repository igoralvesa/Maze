
    return node;
}

int heuristic(int x, int y){
    return abs(endNode->x - x) + abs(endNode->y - y);
}

int is_valid(int x, int y){
    return x >= 0 && y >= 0 && x < rows && y < cols &&
           maze[x][y] != WALL && !visited[x][y];
}

Node *openList[MAX * MAX];
int openSize = 0;

void add_to_open(Node *node){
    for (int i = 0; i < openSize; i++){
        if (openList[i]->x == node->x && openList[i]->y == node->y){
            if (openList[i]->f <= node->f){
                free(node);
                return;
            }else{
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


Node *get_lowest_f_node(){
    int minIndex = 0;
    for (int i = 1; i < openSize; i++){
        if (openList[i]->f < openList[minIndex]->f)
            minIndex = i;
    }
    Node *node = openList[minIndex];
    openList[minIndex] = openList[--openSize];
    return node;
}

void read_maze(const char *filename){
    FILE *file = fopen(filename, "r");
    if (!file){
        perror("Erro ao abrir arquivo");
        exit(1);
    }
    rows = 0;
    cols = 0;
    memset(visited, 0, sizeof(visited));
    openSize = 0;
    startNode = endNode = NULL;

    while (fgets(maze[rows], MAX, file)){
        maze[rows][strcspn(maze[rows], "\n")] = '\0';
        int len = strlen(maze[rows]);
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
    }
    fclose(file);
}

void reconstruct_path(Node *node){
    Node *path[MAX * MAX];
    int count = 0;

    while (node->parent){
        if (maze[node->x][node->y] == FREE)
            maze[node->x][node->y] = '.';
        path[count++] = node;
        node = node->parent;
    }
}

void a_star(){
    add_to_open(startNode);

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    while (openSize > 0){
        Node *current = get_lowest_f_node();

        if (current->x == endNode->x && current->y == endNode->y){
            reconstruct_path(current);
            return;
        }

        visited[current->x][current->y] = 1;

        for (int i = 0; i < 4; i++){
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];

            if (is_valid(nx, ny)){
                int g = current->g + 1;
                int h = heuristic(nx, ny);
                Node *neighbor = create_node(nx, ny, g, h, current);
                add_to_open(neighbor);
            }
        }
    }

    printf("Caminho não encontrado.\n");
}

void save_maze_to_file(const char *filename){
    FILE *file = fopen(filename, "w");
    if (!file){
        perror("Erro ao criar arquivo de saída");
        return;
    }
    for (int i = 0; i < rows; i++){
        fprintf(file, "%s\n", maze[i]);
    }
    fclose(file);
}

void process_all_inputs(){
    DIR *dir = opendir("inputs");
    if (!dir){