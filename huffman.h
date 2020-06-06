#include "SaveLoad.h"
#include <cstring>
#include <sstream>

//NΪ��������Ҷ�ӽڵ���
#define N 512
//SUMΪ�������������ڵ����� 
#define SUM 2 * N - 1
//���弫��ֵ 
#define MAXV 0xffffff
//date�ļ��е������ַ����࣬������λ8λֵ 
#define SIZE 512

typedef struct {
    int weight;
    int parent;
    int LChild;
    int RChild;
} HTNode, HuffmanTree[SUM + 1];

//���������� 
typedef char *HuffmanCode[N + 1];

//ѡ������Ȩֵ��С������ 
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

//������������ 
void CHT(HuffmanTree *ht, int w[], int tw[], int n) {
    int i, j = 0;
    //Ѱ�ҷ����Ȩֵ�����ҶԹ�������Ҷ�ӽڵ㣨���������г�ʼ��
    for (i = 1; i <= n; i++) {
        (*ht)[i].weight = w[tw[i - 1]];
        (*ht)[i].parent = 0;
        (*ht)[i].LChild = 0;
        (*ht)[i].RChild = 0;
    }

    int m = n * 2 - 1;
    //�Է�Ҷ�ӽڵ���г�ʼ��
    for (i = n + 1; i <= m; i++) {
        (*ht)[i].weight = 0;
        (*ht)[i].parent = 0;
        (*ht)[i].LChild = 0;
        (*ht)[i].RChild = 0;
    }

    //�����������
    for (i = n + 1; i <= m; i++) {
        int s1, s2;
        //ѡ������Ȩֵ��С������������������
        Select(ht, i - 1, &s1, &s2);
        //��������Ȩֵ����ѡ��������Ȩֵ֮��
        (*ht)[i].weight = (*ht)[s1].weight + (*ht)[s2].weight;
        //s1������s2������˫�ױ��i
        (*ht)[s1].parent = i;
        (*ht)[s2].parent = i;
        //i���������Һ��ӷֱ�Ϊs1��s2
        (*ht)[i].LChild = s1;
        (*ht)[i].RChild = s2;
    }
}

//�������������� 
void CHC(HuffmanTree *ht, HuffmanCode *hc, int w[], int tw[], int n) {

    char *cd;
    cd = (char *) malloc((n + 1) * sizeof(char));
    cd[n - 1] = '\0';

    int i, start, c, p;
    //��Ҷ�ӽڵ���б���
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