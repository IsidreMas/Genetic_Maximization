# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <math.h>
# include "ran1.h"
# include "bheap.h"

void ExitError(const char *miss, int errcode)
{
    fprintf (stderr, "\nERROR: %s.\nStopping...\n\n", miss);
    exit(errcode);
}


double phi(unsigned x, unsigned y, unsigned z, unsigned t, double a, double b, double c, double d){
    return (1UL<<43) -pow(x-a,2) -pow(y-b,2) -pow(z-c,2) -pow(t-d,2); 
}

double f(unsigned x, unsigned y, unsigned z, unsigned t){
    double s = floor((double)(x+t)/((1UL<<31)+y+z)+0.86525597943226508722);
    return pow(-1,s)*sin(x+y+z+t)
    *phi(x,y,z,t,1237566.4,54783217.5,1237896431.1,325123467.37)
    *phi(x,y,z,t,5674235.4,4067231567.2,13245678.3,3748967543.2)
    *phi(x,y,z,t,3867435523.2,7134893.75,3565897564.1,15675987.34)
    *phi(x,y,z,t,4000223567.09,3734098765.4,3367981234.4,4067231567.25);
}

typedef struct ind{
unsigned x;
unsigned y;
unsigned z;
unsigned t;
double fitness;
}ind;

void mutation1(unsigned int *f, float prob){
    if(uniform() < prob) *f = (*f)^(1U << ((unsigned char) uniform()*8*sizeof(*f)));
}

void BitFlipMutation(unsigned int *f, float prob){
    unsigned char len = 8*sizeof(*f);
    unsigned int mask = 0U;
    register unsigned char i;
    for(i=0; i < len; i++) if(uniform() < prob) *f = (*f)^(1U << i);
}

void OnePointCrossover(unsigned int p1, unsigned int p2,
unsigned int *f1, unsigned int *f2){
/* d \in [1, 8*sizeof(unsigned int)-1] */
unsigned char d = uniform()*(8*sizeof(unsigned int)-1) + 1;
/* d 0's at the beginning and (8*sizeof(unsigned int) - d) 1's at the end */
unsigned int mask = 0xFFFFFFFFU << d;
*f1 = (p1 & mask) | (p2 & ~mask);
*f2 = (p2 & mask) | (p1 & ~mask);
}
void TwoPointCrossover(unsigned int p1, unsigned int p2,
unsigned int *f1, unsigned int *f2){
unsigned char len = 8*sizeof(unsigned int);
unsigned char p = uniform()*(len-2) + 1; /* p \in [1, len-2] */
/* We want a mask with p 0's at the beginning; len-q zeros at the end;
 * and 1's between the positions p and q inclusive
 * where q \in [p+1, len-1].
 * CONSISTENCY NOTE: p <= len-2 ==> p+1 <= len-1
 * THEN: mask = (0xFFFFFFFFU >> (len-q+p)) << p;
 * OBSERVE THAT: p+1 <= len-q+p <= len-1 is a random number
 * THUS: len-q+p = uniform()*(len-p-1) + p+1; */
unsigned int mask = (0xFFFFFFFFU >> (((unsigned char) uniform()*(len-p-1)) + p+1)) << p;
*f1 = (p1 & mask) | (p2 & ~mask);
*f2 = (p2 & mask) | (p1 & ~mask);
}
void UniformCrossover(unsigned int p1, unsigned int p2,
unsigned int *f1, unsigned int *f2,
double prob){
unsigned char len = 8*sizeof(*f);
unsigned int mask = 0U;
register unsigned char i;
for(i=0; i < len; i++) if(uniform() < prob) mask = mask | (1U << i);
*f1 = (p1 & mask) | (p2 & ~mask);
*f2 = (p2 & mask) | (p1 & ~mask);
}

ind tournament_selection(ind *P, unsigned t, unsigned popsize)
{
    ind best = P[(int)(uniform()*popsize)/1];
    ind next;
    for(int i=0;i<=t;i++)
    {
        next = P[(int)(uniform()*popsize)/1];
        if(next.fitness > best.fitness)
        {
            best = next;
        }
    }
    return best;
}

ind tweak(ind individual)
{
    int r = 1000;
    individual.x+=(int)(r*uniform());
    individual.y+=(int)(r*uniform());
    individual.z+=(int)(r*uniform());
    individual.t+=(int)(r*uniform());

    if(individual.x>0)individual.x-=(int)(r*uniform());
    if(individual.y>0)individual.y-=(int)(r*uniform());
    if(individual.z>0)individual.z-=(int)(r*uniform());
    if(individual.t>0)individual.t-=(int)(r*uniform());

    return individual;
}

ind hill_climbing(ind S)
{ 
    ind best = S;
    ind R;
    ind W;
    int n = 2000, iter = 0;
    
    do
    {
        R = tweak(S);
        R.fitness = f(R.x,R.y,R.z,R.t);
        for(int i=0;i<n-1;i++)
        {
            W = tweak(R);
            if((W.fitness=f(W.x,W.y,W.z,W.t))>R.fitness)R=W;
        }
        S=R;
        if(S.fitness>best.fitness)
        {
            best=S;
            printf("Iter= %d Max_fitness= %f\n",iter, best.fitness);
        }
        iter++;
    } while (iter<1000);
    
    return best;
}

ind GA(int popsize, int generations){
    int generation = 0;
    int index;
    int half_popsize = popsize/2;
    ind *P;
    ind *Q;
    ind Parent1;
    ind Parent2;
    ind best;
    if((P=(ind *)malloc(popsize*sizeof(ind)))==NULL)
        ExitError("Couldn't allocate memory for individuals", 32);
    if((Q=(ind *)malloc(popsize*sizeof(ind)))==NULL)
        ExitError("Couldn't allocate memory for individuals", 32);
    randomize();
    for(int i=0; i<popsize; i++)
    {
        P[i].x = UINTran();
        P[i].y = UINTran();
        P[i].z = UINTran();
        P[i].t = UINTran();
    }
    best = P[0];
    do
    {
        Heap *Pq = CreateHeap(popsize);
        for(int i=0;i<popsize;i++)
        {
            insert(Pq,-(P[i].fitness = f(P[i].x,P[i].y,P[i].z,P[i].t)),i);
            if(P[i].fitness > best.fitness)best = P[i];
        }
        
        for(int i=0;i<half_popsize;i++)
        {
            index = half_popsize + i;
            Parent1 = tournament_selection(P,7,popsize);
            Parent2 = tournament_selection(P,7,popsize);
            OnePointCrossover(Parent1.x,Parent2.x,&Q[i].x,&Q[index].x);
            OnePointCrossover(Parent1.y,Parent2.y,&Q[i].y,&Q[index].y);
            OnePointCrossover(Parent1.z,Parent2.z,&Q[i].z,&Q[index].z);
            OnePointCrossover(Parent1.t,Parent2.t,&Q[i].t,&Q[index].t);
            BitFlipMutation(&Q[i].x,0.001);
            BitFlipMutation(&Q[i].y,0.001);
            BitFlipMutation(&Q[i].z,0.001);
            BitFlipMutation(&Q[i].t,0.001);
            BitFlipMutation(&Q[index].x,0.001);
            BitFlipMutation(&Q[index].y,0.001);
            BitFlipMutation(&Q[index].z,0.001);
            BitFlipMutation(&Q[index].t,0.001);
        }
        for(int i=0;i<popsize;i++){
            P[i]=Q[i];
        }
        printf("Generation= %d Max_fitness= %f\n",generation, best.fitness);
        generation++;
        free(Pq->distance);
        free(Pq->node_index);
        free(Pq);
    } while (generation<generations);

    return best;
}

int main (int argc, char *argv[])
{
    int popsize = 20000;
    int generations = 300;
    ind solution = GA(popsize, generations);
    printf("Solution found using Genetic Algorithm:\nx=%u\ny=%u\nz=%u\nt=%u\nfitness=%f\n",solution.x,solution.y,solution.z,solution.t,solution.fitness);
    solution = hill_climbing(solution);
    printf("Solution found using Hill Climber:\nx=%u\ny=%u\nz=%u\nt=%u\nfitness=%f\n",solution.x,solution.y,solution.z,solution.t,solution.fitness);
    unsigned int number= 23U;
    BitFlipMutation(&number, 0.05);
    printf("The result of the mutation is %u", number);
    return 0;
}