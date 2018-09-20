//Gabriel da Silva Alves - TIA 41843827

#include<stdio.h>
#include<stdlib.h>

typedef struct _TNo{
    int w;
    struct _TNo *prox;
}TNo;

//Estrutura para armazenar o grafo
typedef struct{
    int V; // quantidade de vértices
    int A; // quantidade de arestas
    int **vetorNomes;
    TNo **adj; //lista de adjacência
}TGrafo;

//declaracao das funcoes
TGrafo *init( int V );
void insereNo( TGrafo *G, int v, int w);
void mostrarGrafo(TGrafo *G);
void libera(TGrafo *grafo);
int ehCaminho(TGrafo *grafo, int seq[], int N);
void buscaProf(TGrafo *grafo);
void buscaProfRec(TGrafo *grafo, int v, int *visitados);
TGrafo *carregarGrafo(char fileName[]);
void criarVetorNomes(TGrafo *G, int linhas, int coluna);
void insereNome (char *nomeTarefa, int **vetorNomes, int i);
int verificarInsercao(TGrafo *G,int v,int w);
void ordenacaoTop(TGrafo *G);
int achaGrau(TGrafo *G,int v);
char *achaNome (TGrafo *G, int i);

int main (void){
    TGrafo *G;
    char filePath[] = "/Users/gabriel/Documents/grafosMack/grafosMack/ordtopo.txt";
    G = carregarGrafo(filePath);

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
TGrafo *init( int V ){
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
void insereNo( TGrafo *G, int v, int w){
    
    TNo *aux, *ant;
    aux = G->adj[v];
    ant = NULL;
    while( aux && aux->w <= w ){
        if( aux->w == w)
            return;
        
        ant = aux;
        aux = aux->prox;
    }
    TNo *novo = (TNo*)calloc(1, sizeof(TNo));
    novo->w = w;
    novo->prox = aux;
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
void mostrarGrafo(TGrafo *G){
    int i;
    for( i = 1; i< G->V; i++){
        TNo *p = G->adj[i];
        if(p){
            printf("Vertice: (%d)\n",i);
            
            while(p){
                printf("-> %d) %s",p->w, achaNome(G, p->w));
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
    free(grafo->vetorNomes);
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


TGrafo *carregarGrafo(char fileName[]){
    int i;
    int j;
    int quantidadeTarefas;
    int contV;
    
    FILE file = *fopen(fileName,"r");
    char *nomeTarefa = (char*) calloc(100, sizeof(char));

    fgets(nomeTarefa, 100, &file);
    quantidadeTarefas = atoi(nomeTarefa);
    printf("Numero de tarefas: %d \n\n",quantidadeTarefas);
    TGrafo *G;
    G = init(quantidadeTarefas);
    
    criarVetorNomes(G, quantidadeTarefas+1, 100);
    
    
    for(i = 0; i < quantidadeTarefas; i++){
        fgets(nomeTarefa, 100, &file);
        insereNome(nomeTarefa, G->vetorNomes, i+1);
    }
    
    fgets(nomeTarefa, 100, &file);
    contV = atoi(nomeTarefa);
    printf("Numero de arestas: %d\n\n", contV);
    
    for(j = 0; j < contV; j++){
        int v,w;
        if((fscanf(&file,"%d",&v) == EOF) ||  (fscanf(&file,"%d",&w) == EOF)){
            break;
        }
        if( verificarInsercao(G, v, w)){
            insereNo(G,v,w);
        }
    }
    return G;
}

void criarVetorNomes(TGrafo *G, int linhas, int coluna){
    int i,j;
    G->vetorNomes = (int**)malloc(linhas * sizeof(int*));
    
    for (i = 1; i < linhas; i++){
        G->vetorNomes[i] = (int*) malloc(coluna * sizeof(int));
        for (j = 0; j < coluna; j++){
            G->vetorNomes[i][j] = 0;
        }
    }
}

void insereNome(char *nomeTarefa, int **vetorNomes, int i){
    int j;
    for(j = 0;j < 100;j++){
        vetorNomes[i][j] = nomeTarefa[j];
    }
}

int verificarInsercao(TGrafo *G, int v, int w){
    if(v == w){
        printf("Par nao pode ser inserido, vertices iguais");
        return 0;
    }
    if( v > G->V || w > G->V){
        printf("O vertice nao pode ser inserido");
        return 0;
    }
    TNo *p = G->adj[v];
    while(p){
        if(p->w-1 == w ){
            printf("\nTarefas (%d) e (%d) ja inseridas\n",v,w);
            return 0;
        }
        p = p->prox;
    }
    int *visitado = (int *) calloc(G->V,sizeof(int));
    buscaProfRec(G,w,visitado);
    if(visitado[v] == 0){
        return 1;
    } else{
        printf("\nVertice (%d) e (%d) forma ciclo, nao pode ser inserido\n\n",v,w);
        return 0;
    }
}

void ordenacaoTop(TGrafo *G){
    int *graus = (int *)calloc (G->V, sizeof(int));
    int *sequencia = (int *) calloc(G->V, sizeof(int));
    int grauAtual = 0;
    int iteraSequencia = 0;
    int i, countV = G->V;
    
    for(i = 1; i < G->V; i++){
        graus[i] = achaGrau(G, i);
    }
    
    while(countV > 1){
        
        for(i = 1; i < G->V;i++){
            if(graus[i] == grauAtual && sequencia[i] == 0){
                countV --;
                printf("Tarefa %d: %s \n", i, achaNome(G, i));
                sequencia[i] = ++iteraSequencia;
            }
        }
        grauAtual++;
    }
}

int achaGrau(TGrafo *G,int v){
    int i;
    int achaGrau = 0;
    for(i = 1;i < G->V; i++){
        TNo *p = G->adj[i];
        while(p){
            if(p->w == v && i != v){
                achaGrau++;
                break;
            }
            p = p->prox;
        }
    }
    return achaGrau;
}

char *achaNome (TGrafo *G, int i){
    char *nome = (char*)calloc(100, sizeof(char));
    int j;
    
    for (j = 0; j<100; j++){
        nome[j] = G->vetorNomes[i][j];
    }
    return nome;
}
