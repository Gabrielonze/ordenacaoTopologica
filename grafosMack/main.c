//Gabriel da Silva Alves - TIA 41843827

#include<stdio.h>
#include<stdlib.h>

//declaracao das estruturas e tipos
//estrura para representar o no da lista de adjacencia
typedef struct _TNo{
    int w; // aresta (v-w), onde w eh vertice adjacente em relacao a v
    char* taskName;
    struct _TNo *prox;
}TNo;

//Estrutura para armazenar o grafo
typedef struct{
    int V; // quantidade de vértices
    int A; // quantidade de arestas
    TNo **adj; //lista de adjacência
}TGrafo;

//declaracao das funcoes
TGrafo* init( int V );
void insereA( TGrafo *G, int v, int w);
void show(TGrafo *G);
void libera(TGrafo *grafo);
int ehCaminho(TGrafo *grafo, int seq[], int N);
void buscaProf(TGrafo *grafo);
void buscaProfRec(TGrafo *grafo, int v, int *visitados);
TGrafo *carregarGrafo(char fileName[]);
int** criarM(int linhas, int coluna);
void completM (char *taskName, int **mChar, int i);
int checkInsert(TGrafo* D,int v,int w);
void ordenacaoTop(TGrafo* D);
int indeg(TGrafo* D,int v);
char * achaNome (int i);

//Global Vars
int **mChar;

int main (void){
    TGrafo *G;// variavel ponteiro
    char filePath[] = "/Users/gabriel/Documents/grafosMack/grafosMack/ordtopo.txt";
    G = carregarGrafo(filePath);

    //printf("buscaProf=");
    //buscaProf(G);
    
    //show(G);
    ordenacaoTop(G);
    libera(G);
    printf("\n\nfim\n");
    return 0;
}
// implementacao das funcoes
/*
 Função cria e devolve uma nova estrutura TGrafo, inicializa
 os atributos da estrutura, cria uma matriz de matriz de
 adjacência para V vértices e zera os valores para arestas.
 */
TGrafo* init( int V ){
    //cria e devolve uma nova estrutura TGrafo
    TGrafo *grafo=(TGrafo*)calloc(1,sizeof(TGrafo));
    // inicializa os atributos da estrutura,
    grafo->A=0; // zero arestas no momento
    grafo->V=V+1;
    //cria um vetor de listas de adjacencias
    grafo->adj=(TNo**)calloc(grafo->V,sizeof(TNo*));
    return grafo;
}
/*
 Função cria uma aresta v-w no grafo. A função supõe que v e w
 são distintos, positivos e menores que V.
 Se o grafo já tem a aresta v-w, a função não faz nada.
 A função também atualiza a quantidade de arestas no grafo.
 */
void insereA( TGrafo *G, int v, int w){
    
    TNo *aux, *ant;
    aux = G->adj[v];
    ant = NULL;
    while( aux && aux->w <= w ){
        //Se o grafo j· tem a aresta v-w, a funÁ„o n„o faz nada.
        if( aux->w == w)
            return;
        
        ant = aux;
        aux = aux->prox;
    }
    TNo *novo = (TNo*)calloc(1, sizeof(TNo));
    novo->w = w;
    novo->prox = aux;
    novo->taskName = achaNome(w);
    if( ant == NULL)
        G->adj[v] = novo;
    else
        ant->prox = novo;
    G->A++; // aumenta a quantidade de arestas
}
/*
 Para cada vértice v do grafo, esta função imprime, em uma linha,
 todos os vértices adjacentes a v.
 */
void show(TGrafo *G){
    int i;
    for( i = 1; i< G->V; i++){
        TNo *p = G->adj[i];
        if(p){
            printf("Vertice: (%d)\n",i);
            
            while(p){ //Enquanto existirem nÛs para serem exibidos
                printf("-> %d) %s",p->w, p->taskName);
                p = p->prox;
            }
        } else { printf("Vertice: (%d) Vazio\n", i); }
        printf("_____________\n\n");
    }
}
/*
 Função libera a memória utilizado pelo grafo e pela matriz de adjacência.
 */
void libera(TGrafo *grafo){
    int i;
    
    // liberar cada uma das listas
    TNo *aux, *ant;
    for(i=0;i<grafo->V;i++){
        aux = grafo->adj[i];
        while( aux ){
            ant = aux;
            aux = aux->prox;
            free(ant);
        }
    }
    free(grafo->adj);
    free(grafo);
}

void buscaProf(TGrafo *G){
    int *visitados = (int*)calloc(G->V, sizeof(int));
    for (int v = 0; v < G->V; v++){
        if(!visitados[v]){
            buscaProfRec(G, v, visitados);
        }
    }
    printf("\n");
}

void buscaProfRec(TGrafo *G, int v, int *visitados){
    visitados[v] = 1;
    //printf("%d ", v);
    TNo *p = G->adj[v];
    while(p){
        int w = p->w;
        if(!visitados[w]){
            buscaProfRec(G, w, visitados);
        }
        p = p->prox;
    }
}

// ---- Ordenacao topologica


TGrafo *carregarGrafo(char fileName[]){ //Carrega o tgrafo de um arquivo
    int i;
    int j;
    int taskcont;
    int contV;
    int **mChar;
    
    FILE file = *fopen(fileName,"r");
    char *taskName = (char*) calloc(100, sizeof(char));

    fgets(taskName, 100, &file);
    taskcont = atoi(taskName);
    printf("Numero de tarefas: %d \n\n",taskcont);
    
    mChar = criarM(taskcont+1, 100);
    TGrafo *tasks;
    tasks = init(taskcont);
    
    for(i = 0; i < taskcont; i++){
        fgets(taskName, 100, &file);
        completM(taskName, mChar, i+1);
    }
    
    fgets(taskName, 100, &file);
    contV = atoi(taskName);
    printf("Numero de arestas: %d\n\n", contV);
    
    for(j = 0; j < contV; j++){
        int v,w;
        if((fscanf(&file,"%d",&v) == EOF) ||  (fscanf(&file,"%d",&w) == EOF)){
            break;
        }
        //printf("%d ",v);
        //printf("%d ",w);
        if( checkInsert(tasks, v, w)){
            insereA(tasks,v,w);
        }
    }
    return tasks;
}

int** criarM(int linhas, int coluna){ //Cria o vetor com o nome das tarefas
    int i,j;
    mChar = (int**)malloc(linhas * sizeof(int*)); //Apenas aloca os espaÁos de memÛria para o vetor de vetores.
    
    for (i = 1; i < linhas; i++){
        mChar[i] = (int*) malloc(coluna * sizeof(int));
        for (j = 0; j < coluna; j++){
            mChar[i][j] = 0;
        }
    }
    return mChar;
}

void completM(char *taskName, int **mChar, int i){ //Insere os nomes das tarefas vindos de um arquivo para o vetor
    int j;
    for(j = 0;j < 100;j++){
        mChar[i][j] = taskName[j];
    }
}

int checkInsert(TGrafo* D, int v, int w){ //Verifica se um vertice pode ser inserido
    if(v == w){ //N„o pode ter um vÈrtice reflexivo
        printf("Par nao pode ser inserido, vertices iguais");
        return 0;
    }
    if( v > D->V || w > D->V){ //Se um dos nos È maior do que a quantidade total de vÈrtices
        printf("O vertice nao pode ser inserido");
        return 0;
    }
    TNo* p = D->adj[v];
    while(p){ //Enquanto existirem nos
        if(p->w-1 == w ){ //Verifica se estas tarefas j· foram inseridas
            printf("\nTarefas (%d) e (%d) ja inseridas\n",v,w);
            return 0;
        }
        p = p->prox;
    }
    int * visited = (int *) calloc(D->V,sizeof(int));
    buscaProfRec(D,w,visited); //Realiza a busca em profundidade
    if(visited[v] == 0){ //Se v-1 no foi visitado, retorna verdadeiro
        return 1;
    } else{ //Se n„o, eles formam um ciclo e retorna falso, pois n„o podem ser inseridos
        printf("\nVertice (%d) e (%d) forma ciclo, nao pode ser inserido\n\n",v,w);
        return 0;
    }
}

void ordenacaoTop(TGrafo* G){ //Realiza a ordenaÁ„o topolÛgica
    int * graus = (int * )calloc (G->V,sizeof(int));
    int * sequencia = (int *) calloc(G->V,sizeof(int));
    int iteraSequencia =0;
    int i, countV = G->V;
    while(countV > 0){ //Enquanto existirem vÈrtices
        
        for(i = 0;i < G->V;i++){
            graus[i] = indeg(G, i+1); //Seta o grau de [i] como o seu grau de entrada
        }
        
        for(i = 1; i < G->V;i++){
            if(graus[i] == 0 && sequencia[i] == 0){
                countV --; //Remove 1 no contador de vÈrtices, controlando o while
                printf("Tarefa %d: %s \n", i, achaNome(i)); //Exibe a tarefa
                sequencia[i] = ++iteraSequencia;
                //removeV(G,i+1); //Remove o vÈrtice do dÌgrafo
            }
        }
    }
}

int indeg(TGrafo* G,int v){ //Retorna o grau de entrada de um nÛ
    int i;
    int indeg = 0;
    for(i = 0;i < G->V; i++){
        TNo* p = G->adj[i];
        while(p){ //Enquanto existirem nÛs
            if(p->w == v && i != v - 1){ //Se o elemento do nÛ P for V e i for != de v-1
                indeg++; //Aumenta em 1 o grau de entrada
                break;
            }
            p = p->prox;
        }
    }
    return indeg;
}

char* achaNome (int i){ //Retorna o nome de uma atividade
    char * nome = (char*)calloc(100, sizeof(char));
    int j;
    
    for (j = 0; j<100; j++){
        nome[j] = mChar[i][j];
    }
    return nome;
}
