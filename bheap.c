# include <string.h>
# include <stdbool.h>
# include <time.h>
# include <math.h>
# include "bheap.h"

Heap *CreateHeap(unsigned capacity){
    Heap *h = (Heap * ) malloc(sizeof(Heap)); //one is number of heap

    //check if memory allocation is fails
    if(h == NULL){
        printf("Memory Error!");
        exit(0);
    }
    h->count=0;
    h->capacity = capacity;
    h->distance = (double *) malloc(capacity*sizeof(double));
    h->node_index = (unsigned *) malloc(capacity*sizeof(unsigned));//size in bytes

    //check if allocation succeed
    if ( h->distance == NULL){
        printf("Memory Error!");
        exit(0);
    }
    if ( h->node_index == NULL){
        printf("Memory Error!");
        exit(0);
    }
    return h;
}

void insert(Heap *h, double distance, unsigned node_index){
    if( h->count < h->capacity){
        h->distance[h->count] = distance;
        h->node_index[h->count] = node_index;
        heapify_bottom_top(h, h->count);
        h->count++;
    }
}

void heapify_bottom_top(Heap *h,int index){
    double temp1, temp2;
    int parent_node = (index-1)/2;

    if(h->distance[parent_node] > h->distance[index]){
        //swap and recursive call
        temp1 = h->distance[parent_node];
        temp2 = h->node_index[parent_node];
        h->distance[parent_node] = h->distance[index];
        h->node_index[parent_node] = h->node_index[index];
        h->distance[index] = temp1;
        h->node_index[index] = temp2;
        heapify_bottom_top(h,parent_node);
    }
}

void heapify_top_bottom(Heap *h, int parent_node){
    int left = parent_node*2+1;
    int right = parent_node*2+2;
    int min;
    double temp1, temp2;

    if(left >= h->count || left <0)
        left = -1;
    if(right >= h->count || right <0)
        right = -1;

    if(left != -1 && h->distance[left] < h->distance[parent_node])
        min=left;
    else
        min =parent_node;
    if(right != -1 && h->distance[right] < h->distance[min])
        min = right;

    if(min != parent_node){
        temp1 = h->distance[min];
        temp2 = h->node_index[min];
        h->distance[min] = h->distance[parent_node];
        h->node_index[min] = h->node_index[parent_node];
        h->distance[parent_node] = temp1;
        h->node_index[parent_node] = temp2;

        // recursive  call
        heapify_top_bottom(h, min);
    }
}

unsigned PopMin(Heap *h){
    unsigned pop;
    if(h->count==0){
        printf("\n__Heap is Empty__\n");
        return -1;
    }
    // replace first node by last and delete last
    pop = h->node_index[0];
    h->distance[0] = h->distance[h->count-1];
    h->node_index[0] = h->node_index[h->count -1];
    h->count--;
    heapify_top_bottom(h, 0);
    return pop;
}

void decreasePriority(Heap *h, double distance, unsigned node_index)
{
    int i;
    for (i = 0; i < h->count; i++)
        if (node_index == h->node_index[i])break;
    h->distance[i] = distance;

    if(i==0)return;
    if (h->distance[i]<h->distance[(i-1)/2]) heapify_bottom_top(h, i);
    else heapify_top_bottom(h, (i-1)/2);
}

void print(Heap *h){
    int i;
    printf("____________Print Heap_____________\n");
    for(i=0;i< h->count;i++){
        printf("-> %f ",h->distance[i]);
    }
    printf("->__/\\__\n");
}