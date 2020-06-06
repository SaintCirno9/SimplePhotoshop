#include "SaveLoad.h"
#include <cstring>
#include <sstream>

//N为汉夫曼树叶子节点数
#define N 512
//SUM为哈夫曼树所含节点总数 
#define SUM 2 * N - 1
//定义极大值 
#define MAXV 0xffffff
//date文件中的所含字符种类，本程序位8位值 
#define SIZE 512

typedef struct {
    int weight;
    int parent;
    int LChild;
    int RChild;
} HTNode, HuffmanTree[SUM + 1];

//哈夫曼编码 
typedef char *HuffmanCode[N + 1];

//选出两个权值最小的子树 
void Select(HuffmanTree *ht, int m, int *s1, int *s2) {
    int minWeight = MAXV;
    int i;
    for (i = 1; i <= m; i++) {
        if ((*ht)[i].weight < minWeight && (*ht)[i].parent == 0) {
            minWeight = (*ht)[i].weight;
            *s1 = i;
        }
    }

    minWeight = MAXV;
    for (i = 1; i <= m; i++) {
        if ((*ht)[i].weight < minWeight && (*ht)[i].parent == 0 && i != *s1) {
            minWeight = (*ht)[i].weight;
            *s2 = i;
        }
    }
}

//创建汉夫曼树 
void CHT(HuffmanTree *ht, int w[], int tw[], int n) {
    int i, j = 0;
    //寻找非零的权值，并且对哈夫曼树叶子节点（子树）进行初始化
    for (i = 1; i <= n; i++) {
        (*ht)[i].weight = w[tw[i - 1]];
        (*ht)[i].parent = 0;
        (*ht)[i].LChild = 0;
        (*ht)[i].RChild = 0;
    }

    int m = n * 2 - 1;
    //对非叶子节点进行初始化
    for (i = n + 1; i <= m; i++) {
        (*ht)[i].weight = 0;
        (*ht)[i].parent = 0;
        (*ht)[i].LChild = 0;
        (*ht)[i].RChild = 0;
    }

    //构造哈夫曼树
    for (i = n + 1; i <= m; i++) {
        int s1, s2;
        //选择连个权值最小的子树，创建新子树
        Select(ht, i - 1, &s1, &s2);
        //新子树的权值等于选出来两个权值之和
        (*ht)[i].weight = (*ht)[s1].weight + (*ht)[s2].weight;
        //s1子树和s2子树的双亲变成i
        (*ht)[s1].parent = i;
        (*ht)[s2].parent = i;
        //i子树的左右孩子分别为s1和s2
        (*ht)[i].LChild = s1;
        (*ht)[i].RChild = s2;
    }
}

//创建哈夫曼编码 
void CHC(HuffmanTree *ht, HuffmanCode *hc, int w[], int tw[], int n) {

    char *cd;
    cd = (char *) malloc((n + 1) * sizeof(char));
    cd[n - 1] = '\0';

    int i, start, c, p;
    //对叶子节点进行编码
    for (i = 1; i <= n; i++) {

        start = n - 1;
        c = i;
        p = (*ht)[i].parent;

        while (p != 0) {
            --start;

            if ((*ht)[p].LChild == c) cd[start] = '0';
            else cd[start] = '1';
            c = p;
            p = (*ht)[p].parent;
        }

        (*hc)[tw[i - 1]] = (char *) malloc((n - start) * sizeof(char));

        strcpy((*hc)[tw[i - 1]], &cd[start]);
    }
}


int GetWeightId(int tw[], int w[]) {
    int i, n = 0;
    for (i = 0; i < SIZE; i++) {
        if (w[i] != 128) {
            tw[n++] = i;
        }
    }
    return n;
}