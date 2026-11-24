#include <stdio.h>
#include <stdlib.h>

// struct p/ representar um nó na lista de adjacência
typedef struct No {
    int destino;     // sensor vizinho
    int peso;        // custo ou distância da conexão
    struct No* prox; // ponteiro para o prox vizinho na lista encadeada
} No;

// struct principal do grafo
typedef struct {
    No** lista;      // vetor que cada posição aponta para a lista de vizinhos de um sensor
    int numVertices; // qtd total de sensores (vértices)
    int numArestas;  // qtd total de conexões (arestas)
} Grafo;


// CRIAR 
//função que cria o grafo na memória
Grafo* criarGrafo(int v) {
    Grafo* g = malloc(sizeof(Grafo));   // alocação de espaço para a "caixa" do grafo
    g->numVertices = v;                 // guarda o número de sensores informado
    g->numArestas = 0;                  // começando sem nenhuma conexão 

    g->lista = malloc(v * sizeof(No*)); // cria o vetor p/ guardar as listas
    for (int i = 0; i < v; i++)         // olha todas as posições do vetor
        g->lista[i] = NULL;             // define que cada lista começa vazia 

    return g;                           // devolve o grafo criado e limpo
}

// ADICIONAR ARESTA
// função pra ligar dois sensores (o e d) com um peso (p)
void adicionarAresta(Grafo* g, int o, int d, int p) {
    // cria o elo de ida: tipo  sensor 'o' conhece o sensor 'd'
    No* n1 = malloc(sizeof(No));      // cria um papelzinho de conexão novo
    n1->destino = d;                  // escreve que o destino é 'd'
    n1->peso = p;                     // escreve o peso 'p'
    n1->prox = g->lista[o];           // faz o novo nó apontar para o antigo primeiro da lista
    g->lista[o] = n1;                 // o sensor 'o' agora tem esse novo nó no topo da lista

    // cria o elo de volta: sensor 'd' conhece o sensor 'o'
    No* n2 = malloc(sizeof(No));      // cria outro papelzinho de conexão
    n2->destino = o;                  // escreve que o destino é 'o'
    n2->peso = p;                     // escreve o mesmo peso 'p'
    n2->prox = g->lista[d];           // faz o novo nó apontar para o antigo primeiro da lista
    g->lista[d] = n2;                 // o sensor 'd' agora tem esse novo nó no topo da lista

    g->numArestas++;                  // mais uma conexão no total
}


// CARREGAR
// leitura das informações de um arquivo de texto e monta o grafo
Grafo* carregarArquivo(char nome[]) {
    FILE* f = fopen(nome, "r");         // abre o arquivo para leitura ("r")
    if (!f) return NULL;                // se der erro ao abrir = stop

    int v, a;                           // variáveis para guardar sensores e arestas
    fscanf(f, "%d %d", &v, &a);         // leitura da primeira linha do arquivo

    Grafo* g = criarGrafo(v);           // usa a função criarGrafo com o tamanho lido

    for (int i = 0; i < a; i++) {       // repete para cada conexão descrita no arquivo
        int o, d, p;                    // variáveis para origem, destino e peso
        fscanf(f, "%d %d %d", &o, &d, &p); // leitura dos dados da conexão atual
        adicionarAresta(g, o, d, p);    // chama a função para amarrar os sensores
    }

    fclose(f);                          // fecha o arquivo 
    return g;                           // retorna o grafo todo montado
}


// IMPRIMIR
//mostra o grafo na tela de forma visual
void imprimir(Grafo* g) {
    for (int i = 0; i < g->numVertices; i++) { // passa por cada sensor
        printf("Sensor %d -> ", i);             // imprime o nome do sensor
        No* t = g->lista[i];                    // pega a lista de amigos dele
        while (t) {                            
            printf("(%d,%d) ", t->destino, t->peso); // imprime (vizinho, peso)
            t = t->prox;                        //próximo vizinho
        }
        printf("\n");                           
    }
}


// Busca em Largura
//encontrar o caminho mais curto (em número de saltos) entre início e fim
void bfs(Grafo* g, int inicio, int fim) {
    int* vis = calloc(g->numVertices, sizeof(int));   // vetor para marcar quem já visitamos
    int* fila = malloc(g->numVertices * sizeof(int)); // fila para saber quem checar primeiro
    int* pai = malloc(g->numVertices * sizeof(int));  // para lembrar o caminho de volta

    for (int i = 0; i < g->numVertices; i++) pai[i] = -1; //inicializa todos sem "pai"

    int ini = 0, f = 0; // controles da fila (início e fim)
    vis[inicio] = 1;    // marca o sensor de partida como visitado
    fila[f++] = inicio; // coloca ele no começo da fila

    while (ini < f) {   
        int u = fila[ini++]; // tira o primeiro da fila para analisar
        No* t = g->lista[u]; // pega os vizinhos de 'u'

        while (t) {          // oercorre todos os vizinhos de 'u'
            if (!vis[t->destino]) {   // se ainda não visitei esse vizinho...
                vis[t->destino] = 1;  // marcação de visita agora
                pai[t->destino] = u;  // anoto que cheguei nele vindo de 'u'
                fila[f++] = t->destino; // coloco ele na fila para checar os vizinhos dele depois
            }
            t = t->prox;              // pula para o próximo vizinho da lista
        }
    }

    if (!vis[fim]) {                  // se depois não houve marcação de destino
            printf("Sem caminho\n");  // = não tem como chegar 
        return;                       // encerra a função
    }

    int* caminho = malloc(g->numVertices * sizeof(int)); // vetor para guardar o caminho final
    int tam = 0;                      // tamanho do caminho

    for (int v = fim; v != -1; v = pai[v]) // faz o caminho de volta = do fim para o início
        caminho[tam++] = v;           // guarda os sensores no vetor

    printf("Caminho: ");              // começa a imprimir o resultado
    for (int i = tam - 1; i >= 0; i--) { // imprime de trás para frente (início -> fim)
        printf("%d", caminho[i]);     // imprime o número do sensor
        if (i > 0) printf(" -> ");    // coloca a setinha se não for o último
    }
    printf("\nSaltos: %d\n", tam - 1); // quantidade de conexões usadas no caminho
}


// CONECTIVIDADE
// verificação se todos os sensores conseguem se falar (diretamente ou indiretamente)
int estaConectado(Grafo* g) {
    int* vis = calloc(g->numVertices, sizeof(int));   // marca quem visitamos
    int* fila = malloc(g->numVertices * sizeof(int)); // fila para busca

    int ini = 0, f = 0; // indices da fila
    vis[0] = 1;         // tenta começar pelo sensor 0
    fila[f++] = 0;      // coloca ele na fila

    while (ini < f) {   //mesma lógica da Busca em largura: percorre tudo o que conseguir
        int u = fila[ini++];
        No* t = g->lista[u];
        while (t) {
            if (!vis[t->destino]) {
                vis[t->destino] = 1;
                fila[f++] = t->destino;
            }
            t = t->prox;
        }
    }

    for (int i = 0; i < g->numVertices; i++) // verifica todos os sensores do grafo
        if (!vis[i]) return 0;               // se achar um que não foi visitado = retorna "Falso" (0)

    return 1; // se visitou todos = retorna "Verdadeiro" (1)
}


// GRAU MÉDIO
//calcula a média de conexões por sensor
void grauMedio(Grafo* g) {
    // 2 * arestas -> pq cada aresta liga dois sensores (conta como conexão para ambos)
    printf("Grau medio: %.2f\n", (2.0 * g->numArestas) / g->numVertices);
}
 

//  DFS ARTICULAÇÃO (Pontos Fracos)
// função p/ encontrar sensores que, se falharem, dividem o grafo em dois
void dfsAP(Grafo* g, int u, int* vis, int* disc, int* low, int* parent, int* ap, int* tempo) {
    vis[u] = 1;                      // marca o sensor atual como visitado
    disc[u] = low[u] = ++(*tempo);   // define o tempo de entrada no sensor
    int filhos = 0;                  // conta quantos "filhos" ele terá na busca

    No* t = g->lista[u];             // pega os vizinhos dele
    while (t) {
        int v = t->destino;          // identifica o vizinho atual

        if (!vis[v]) {               // se o vizinho ainda não foi visitado
            filhos++;                // aumenta o contador de filhos
            parent[v] = u;           // define 'u' como pai de 'v'
            dfsAP(g, v, vis, disc, low, parent, ap, tempo); // recursão

            if (low[v] < low[u]) low[u] = low[v]; //atualiza o caminho alternativo mais baixo

            //verifica se o sensor 'u' é um ponto de articulação
            if (parent[u] == -1 && filhos > 1) ap[u] = 1;          // caso seja a raiz e tenha filhos
            if (parent[u] != -1 && low[v] >= disc[u]) ap[u] = 1;    // caso comum
        }
        else if (v != parent[u]) {   // se o vizinho ja recebeu visita e não é o pai
            if (disc[v] < low[u]) low[u] = disc[v]; // atualiza o low com o tempo de descoberta
        }
        t = t->prox;                 // próx vizinho da lista
    }
}

// função p/ verificar se existe algum sensor que é ponto de articulação
int temArticulacao(Grafo* g) {
    int v = g->numVertices;           // pega o número total de sensores
    int* vis = calloc(v, sizeof(int)); // aloca vetor de visitados
    int* disc = malloc(v * sizeof(int)); // vetor de tempos de descoberta
    int* low = malloc(v * sizeof(int));  // vetor de menores tempos alcançáveis
    int* parent = malloc(v * sizeof(int)); // vetor de pais
    int* ap = calloc(v, sizeof(int));    // vetor que marca se é articulação = 1 ou não = 0

    for (int i = 0; i < v; i++) parent[i] = -1; // inicializa todos sem pai

    int tempo = 0; // começa o relógio em zero

    for (int i = 0; i < v; i++)   // garante que percorre todos os pedaços do grafo
        if (!vis[i])
            dfsAP(g, i, vis, disc, low, parent, ap, &tempo);

    for (int i = 0; i < v; i++)   // verificação de resultado se alguém foi marcado
        if (ap[i]) return 1;      // se achou um sensor crítico, retorna "Sim"

    return 0; // se não achou nenhum, retorna "Não"
}



// DFS PONTES (Conexões Fracas)
//função pra encontrar conexões que se forem cortadas, isolam partes do grafo
void dfsBridge(Grafo* g, int u, int* vis, int* disc, int* low, int* parent, int* bridge, int* tempo) {
    vis[u] = 1;                      // marca o sensor atual
    disc[u] = low[u] = ++(*tempo);   // marca o tempo

    No* t = g->lista[u];             // olha os vizinhos
    while (t) {
        int v = t->destino;

        if (!vis[v]) {               // se o vizinho for novo
            parent[v] = u;           // define o pai
            dfsBridge(g, v, vis, disc, low, parent, bridge, tempo); // vai mais fundo

            if (low[v] < low[u]) low[u] = low[v]; // atualiza o valor de low

            if (low[v] > disc[u]) *bridge = 1; // se o vizinho não tem volta = esta conexão é uma ponte
        }
        else if (v != parent[u]) {   // se já visitou e não é pai
            if (disc[v] < low[u]) low[u] = disc[v];
        }
        t = t->prox;                 // próx da lista
    }
}

//função p/ verificar se o grafo tem alguma conexão "ponte"
int temPonte(Grafo* g) {
    int v = g->numVertices;
    int* vis = calloc(v, sizeof(int));
    int* disc = malloc(v * sizeof(int));
    int* low = malloc(v * sizeof(int));
    int* parent = malloc(v * sizeof(int));
    int tempo = 0, bridge = 0; // variável bridge começa como zero (falso)

    for (int i = 0; i < v; i++) parent[i] = -1;

    for (int i = 0; i < v; i++)
        if (!vis[i])
            dfsBridge(g, i, vis, disc, low, parent, &bridge, &tempo);

    return bridge; // devolve se achou alguma ponte
}


//  COPIAR
// cria um grafo novo idêntico ao original
Grafo* copiarGrafo(Grafo* g) {
    Grafo* novo = criarGrafo(g->numVertices); // cria um grafo com mesmo número de sensores

    for (int i = 0; i < g->numVertices; i++) { // percorre todos os sensores
        No* t = g->lista[i];                    // pega os amigos de 'i'
        while (t) {
            if (i < t->destino)                 // adiciona só uma vez (evita duplicar arestas)
                adicionarAresta(novo, i, t->destino, t->peso);
            t = t->prox;                        // próx amigo
        }
    }
    return novo; // retorna o novo grafo copiado
}

// REMOVER VÉRTICE TEMP 
// apaga um sensor temporariamente (usado nos cálculos de K)
void removerVerticeTemp(Grafo* g, int v) {
    for (int i = 0; i < g->numVertices; i++) { // olha cada sensor do grafo
        No* t = g->lista[i];                    // pega a lista de vizinhos dele
        No* ant = NULL;                         // variável para o vizinho anterior

        while (t) {                             // percorre a lista de vizinhos
            if (t->destino == v) {              // se o vizinho for justamente o sensor 'v'
                if (ant) ant->prox = t->prox;   // faz o anterior pular o 'v'
                else g->lista[i] = t->prox;     // se for o primeiro da lista, o segundo assume
                free(t);                        // apaga o nó da memória
                break;                          // sai do loop do sensor atual
            }
            ant = t;                            // atualiza o anterior
            t = t->prox;                        // segue para o próx
        }
    }
    g->lista[v] = NULL;                         // garante que a lista do próprio 'v' está vazia
}


// K-CONNECTIVIDADE
// funçaõ p/ medir a resistência do sistema contra falhas
int kVertices(Grafo* g) {
    if (temArticulacao(g)) return 1; // se existe um sensor crítico, a resistência é 1

    int V = g->numVertices;           // número total de sensores

    for (int i = 0; i < V; i++) {     // testa remover o sensor 'i'
        for (int j = i + 1; j < V; j++) { // e também o sensor 'j'
            Grafo* temp = copiarGrafo(g);  // faz uma cópia para não estragar o oficial
            removerVerticeTemp(temp, i);   // tira o primeiro sensor
            removerVerticeTemp(temp, j);   // tira o segundo sensor

            if (!estaConectado(temp)) return 2; // se quebrou a conexão com 2 saindo = resistência é 2
        }
    }
    return 3; // se resistiu a tirar 2 sensores = resistência é pelo menos 3
}

// função p/ medir a resistência baseada nas conexões (arestas)
int kArestas(Grafo* g) {
    if (temPonte(g)) return 1; // se existe uma conexão crítica = a resistência é 1
    return 2; // caso contrário = é pelo menos 2
}

// p/ mostrar o resultado final da resistência na tela
void mostrarK(Grafo* g) {
    int kv = kVertices(g); // chama o cálculo para sensores
    int ke = kArestas(g);  // chama o cálculo para conexões

    printf("Conectividade por vertices: %d\n", kv);
    printf("Conectividade por arestas: %d\n", ke);
    printf("Sao necessarios %d sensores ou %d enlaces para desconectar.\n", kv, ke);
}

// CLUSTERING 
// calcula o quão "unidos" são os vizinhos de um sensor
void clustering(Grafo* g, int v) {
    int viz[100], n = 0;            // vetor para guardar vizinhos e contador 'n'
    No* t = g->lista[v];            // pega os vizinhos do sensor 'v'

    while (t) {                     // enquanto existir vizinhos
        viz[n++] = t->destino;      // guarda o número do vizinho no vetor
        t = t->prox;                // vai para o próx vizinho
    }

    int lig = 0;                                // contador de amizades entre os vizinhos
    int poss = n * (n - 1) / 2;                 // fórmula de quantas amizades poderiam existir

    for (int i = 0; i < n; i++) {               // pega cada vizinho da lista
        No* temp = g->lista[viz[i]];            // olha os amigos desse vizinho
        while (temp) {                          // percorre os amigos desse vizinho
            for (int j = i + 1; j < n; j++)     // vê se ele é amigo de outro vizinho de 'v'
                if (temp->destino == viz[j]) lig++; // se sim, conta mais uma ligação
            temp = temp->prox;
        }
    }

    printf("CC = %.2f\n", (float) lig / poss);  // mostra a proporção (amizades reais / possíveis)
}



// REMOVER NÓ DEFINITIVO
// remoção de um sensor do sistema para sempre e atualiza os dados
void removerNo(Grafo* g, int v) {
    int removidas = 0; // contador de conexões que vão sumir

    for (int i = 0; i < g->numVertices; i++) { // passa por todos os sensores
        No* t = g->lista[i];                    // pega os vizinhos
        No* ant = NULL;

        while (t) {
            if (t->destino == v) {              // se achar conexão com o excluído...
                if (ant) ant->prox = t->prox;   // remove da lista
                else g->lista[i] = t->prox;
                free(t);                        // apaga
                removidas++;                    // conta como conexão removida
                break;
            }
            ant = t;
            t = t->prox;
        }
    }

    g->lista[v] = NULL;       // limpa a lista do próprio sensor excluído
    g->numArestas -= removidas; // diminui o total de arestas do grafo
    g->numVertices--;           // diminui o total de sensores ativos

    // calcula e mostra o novo grau médio após a exclusão
    printf("Novo grau medio: %.2f\n", (2.0 * g->numArestas) / g->numVertices);
}


// MAIN
int main() {
    Grafo* g = NULL; // começa o programa sem nenhum grafo na memória
    int op;          // variável para guardar a opção escolhida no menu

    do {
        // impressão das opções na tela
        printf("\n1-Carregar\n2-Imprimir\n3-Caminho\n4-Grau medio\n5-K conectividade\n6-Clustering\n7-Remover no\n0-Sair\n");
        printf("Escolha: ");
        scanf("%d", &op); // lê a opção do usuário

        if (op == 1) {             // se escolheu 1 = Carregar
            char nome[50];         // espaço para o nome do arquivo
            printf("Arquivo: ");
            scanf("%s", nome);     // lê o nome do arquivo
            g = carregarArquivo(nome); // carrega o grafo
        }
        else if (op == 2) imprimir(g); // se 2, mostra o grafo na tela
        else if (op == 3) {            // se 3, calcula caminho (BFS)
            int s;
            printf("Origem: ");
            scanf("%d", &s);           // lê o sensor de partida
            bfs(g, s, g->numVertices - 1); // busca até o último sensor cadastrado
        }
        else if (op == 4) grauMedio(g); // se 4, mostra a média de amigos
        else if (op == 5) mostrarK(g);   // se 5, mostra a força do sistema
        else if (op == 6) {              // se 6, calcula o agrupamento
            int v;
            printf("Sensor: ");
            scanf("%d", &v);
            clustering(g, v);
        }
        else if (op == 7) {              // se 7, remove um sensor
            int v;
            printf("Remover: ");
            scanf("%d", &v);
            removerNo(g, v);
        }

    } while (op != 0); // continua rodando enquanto o usuário não digitar 0 = Sair

    return 0;
}