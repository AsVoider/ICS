/*
* name : GJX
* ID : 521021910197
*/
#include "cachelab.h"
#include<stdlib.h>
#include<getopt.h>
#include<stdio.h>
#include<string.h>
char trace[500];
int hit = 0, miss = 0, evi = 0;
int ifView = 0;
int s, E, b;
typedef struct FindCache
{
    int S;
    int E;
    int B;
}fC;

typedef struct Line
{
    int valid;
    int tag;
    int stremp;
}line;


typedef struct Cache
{
    fC FC;
    line **cacheline;
}cache;

cache *newCache = NULL;

void update(unsigned address)
{
    int tag = address >> (s + b);
    int set = (address >> b) & (((unsigned)(-1)) >> (32 - s));
    for(int i = 0; i < (1 << s); i++)
    {
        for(int j = 0; j < E; j++)
        {
            if(newCache->cacheline[i][j].valid)
                newCache->cacheline[i][j].stremp++;
        }
    }//update time

    for(int i = 0; i < newCache->FC.E; i++)
    {
        if(newCache->cacheline[set][i].valid && newCache->cacheline[set][i].tag == tag)
        {
            hit++;
            newCache->cacheline[set][i].stremp = 0;
            return;
        }
    }//hit

    for(int i = 0; i < E; i++)
    {
        if(newCache->cacheline[set][i].valid == 0)
        {
            miss++;
            newCache->cacheline[set][i].valid = 1;
            newCache->cacheline[set][i].tag = tag;
            newCache->cacheline[set][i].stremp = 0;
            return;
        }
    }//cold miss

    evi++;
    miss++;
    int maxE = -1;
    int maxNow = -1;
    for(int i = 0; i < E; i++)
    {
        if(newCache->cacheline[set][i].stremp > maxNow)
        {
            maxNow = newCache->cacheline[set][i].stremp;
            maxE = i;
        }
    }
    newCache->cacheline[set][maxE].tag = tag;
    newCache->cacheline[set][maxE].stremp = 0;
    //eviction
    return;
}

void freeCache()
{
    int S = 1 << s;
    for(int i = 0; i < S; i++)
    {
        free(newCache->cacheline[i]);
    }
    free(newCache->cacheline);
    free(newCache);
}

int main(int argc, char*argv[])
{
    /*receive s E b t*/   
    if('s' != getopt(argc, argv, "s:E:b:t:"))
    {
        printf("wrong input\n");
        return 0;
    }
    s = atoi(optarg);
    if('E' != getopt(argc, argv, "s:E:b:t:"))
    {
        printf("wrong input\n");
        return 0;
    }
    E = atoi(optarg);
    if('b' != getopt(argc, argv, "s:E:b:t:"))
    {
        printf("wrong input\n");
        return 0;
    }
    b = atoi(optarg);
    if('t' != getopt(argc, argv, "s:E:b:t:"))
    {
        printf("wrong input\n");
        return 0;
    }
    
    //printf("here");
    /*init cache*/
    newCache = (cache*)malloc(sizeof(cache));
    int S = 1 << s, B = 1 << b;
    newCache->FC.S = S; 
    newCache->FC.E = E; 
    newCache->FC.B = B;
    newCache->cacheline = (line**)malloc(sizeof(line*) * S);
    for(int i = 0; i < S; i++)
    {
        newCache->cacheline[i] = (line*)malloc(sizeof(line) * E);
        for(int j = 0; j < E; j++)
        {
            newCache->cacheline[i][j].valid = 0;
            newCache->cacheline[i][j].stremp = 0;
            newCache->cacheline[i][j].tag = -1;
        }
    }
    
    /*open files*/
    FILE* File;
    if(NULL == (File = fopen(optarg, "r")))
    {
        printf("no such file or directory\n");
        return 0;
    }
    
    char command;//L or S or M
    unsigned addr;//address
    int numSize;//block
    
    while(fscanf(File, " %c %x,%d", &command, &addr, &numSize) > 0)
    {
        /*M Twice*/
        if(command == 'M')
        {
            update(addr);
            update(addr);
        }
        /*L or S once*/
        else if(command == 'L')
        {
            update(addr);
        }
        else if(command == 'S')
        {
            update(addr);
        }
    }
    fclose(File);
    freeCache();
    //parse Command Line
    printSummary(hit, miss, evi);
    return 0;
}
