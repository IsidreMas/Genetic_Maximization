#include <stdio.h>
#include <stdlib.h>

struct Heap{
    double *distance;
    unsigned *node_index;
    int count;
    int capacity;
};
typedef struct Heap Heap;

Heap *CreateHeap(unsigned capacity);
void insert(Heap *h, double distance, unsigned node_index);
void print(Heap *h);
void heapify_bottom_top(Heap *h,int index);
void heapify_top_bottom(Heap *h, int parent_node);
unsigned PopMin(Heap *h);
void decreasePriority(Heap *h, double distance, unsigned node_index);