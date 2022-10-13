#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define TRUE 1
#define FALSE 0

#define MAX_VERTICES 100
#define INF 999

int* parent;

struct Edge {			// 간선을 나타내는 구조체
    int start, end, weight;
};

typedef struct GraphType {
    int n;               // 정점의 개수
    int n_e;            // 간선의 개수
    int** adj_mat;         // 인접 행렬
    struct Edge* edges;      // 간선 정보
}GraphType;


void set_init(int n);
int set_find(int curr);
void set_union(int a, int b);
void graph_init(GraphType* g);
void insert_edge(GraphType* g, int start, int end, int w);
int compare(const void* a, const void* b);
void kruskal(GraphType* g);


int main(void)
{
    GraphType* g;
    g = (GraphType*)malloc(sizeof(GraphType));


    int num_node, i, j, max_e;
    printf("n을 입력: ");
    scanf_s("%d", &num_node);

    g->n = num_node;
    g->n_e = 0; //?

    max_e = num_node * num_node / 2;
    g->edges = (struct Edge*)malloc(sizeof(struct Edge) * max_e);
    for (i = 0; i < max_e; i++) {
        g->edges[i].start = g->edges[i].end = 0;
        g->edges[i].weight = INF;
    }


    //2차원 행렬 생성 n x n 크기의 행렬 (n은 노드의 개수)
    g->adj_mat = (int**)malloc(sizeof(int*) * num_node);      // 인접행렬 표현
    for (i = 0; i < num_node; i++) {
        g->adj_mat[i] = (int*)malloc(sizeof(int) * num_node);
        for (j = 0; j < num_node; j++)
            g->adj_mat[i][j] = 0; //인접행렬 초기화
    }

    //사용자로부터 인접행렬 정보 입력받기
    for (i = 0; i < num_node; i++) {
        for (j = 0; j < num_node; j++)
            scanf_s("%d", &(g->adj_mat[i][j]));
    }

    // 인접행렬에서 LL을 이용한 간선 리스트 만들기 for Kruskal alg.
    for (i = 0; i < num_node; i++) {
        for (j = 0; j < i; j++)
            if (g->adj_mat[i][j] > 0 && g->adj_mat[i][j] != INF) {   // only valid edges
                g->edges[g->n_e].start = i;   // 노드 번호는 1부터 시작
                g->edges[g->n_e].end = j;
                g->edges[g->n_e++].weight = g->adj_mat[i][j];
            }
    }

    printf("\n그래프 인접행렬 표현: \n");
    for (i = 0; i < num_node; i++) {
        for (j = 0; j < num_node; j++)
            printf("%3d ", g->adj_mat[i][j]);
        printf("\n");
    }
    printf("\n");

    int st_kind;
    printf("옵션 선택 최소 0 최대 1 :");
    scanf_s("%d", &st_kind);
    kruskal(g,st_kind);
    free(g);
    return 0;
}

// 초기화
void set_init(int n)
{
    parent = (int*)malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++)
        parent[i] = -1;
}


// 그래프 초기화 
void graph_init(GraphType* g)
{
    g->n = 0;
    for (int i = 0; i < 2 * MAX_VERTICES; i++) {
        g->edges[i].start = 0;
        g->edges[i].end = 0;
        g->edges[i].weight = INF;
    }
}

// curr가 속하는 집합을 반환한다.
int set_find(int curr)
{
    if (parent[curr] == -1)
        return curr; 			// 루트 
    while (parent[curr] != -1) curr = parent[curr];
    return curr;
}

void set_union(int a, int b)
{
    int root1 = set_find(a);	// 노드 a의 루트를 찾는다. 
    int root2 = set_find(b);	// 노드 b의 루트를 찾는다. 
    if (root1 != root2) 		// 합한다. 
        parent[root1] = root2;
}

// 간선 삽입 연산
void insert_edge(GraphType* g, int start, int end, int w)
{
    g->edges[g->n].start = start;
    g->edges[g->n].end = end;
    g->edges[g->n].weight = w;
    g->n++;
}

//오름차순 정렬용 => 최소 비용
int compare1(const void* a, const void* b)
{
    struct Edge* x = (struct Edge*)a;
    struct Edge* y = (struct Edge*)b;
    return (x->weight - y->weight);
}

//내림차순 정렬용 => 최대 비용
int compare2(const void* a, const void* b)
{
    struct Edge* x = (struct Edge*)a;
    struct Edge* y = (struct Edge*)b;
    return (y->weight - x->weight);
}


// kruskal의 최소 비용 신장 트리 프로그램
void kruskal(GraphType* g,int st_kind)
{
    int edge_accepted = 0;	// 현재까지 선택된 간선의 수	
    int uset, vset;		// 정점 u와 정점 v의 집합 번호
    struct Edge e;

    set_init(g->n);		// 집합 초기화
    switch (st_kind) {
    case 0://0은 최소비용
        qsort(g->edges, g->n, sizeof(struct Edge), compare1);
        break;
    case 1:
        qsort(g->edges, g->n, sizeof(struct Edge), compare2);
    }
    

    printf("크루스칼 최소 신장 트리 알고리즘 \n");
    int i = 0;
    while (edge_accepted < g->n_e-1)	// 신장트리 간선의 수 = (n-1)
    {
        e = g->edges[i];
        uset = set_find(e.start);	// 정점 u의 집합 번호 
        vset = set_find(e.end);		// 정점 v의 집합 번호
        if (uset != vset) {		// 서로 속한 집합이 다르면
            printf("간선 (%d,%d) %d 선택\n", e.start, e.end, e.weight);
            edge_accepted++;
            set_union(uset, vset);	// 두개의 집합을 합친다.
        }
        i++;
    }
}