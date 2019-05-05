#include <stdlib.h>
#include <stdio.h>
#include "cachelab.h"
#include <getopt.h>
#include <math.h>
#include <time.h>
//maketypedef struct cache_struct* cacheline;
typedef struct  {
	char vbit;
	unsigned long long int tag;
	unsigned long long int lru;
}cache_line;

cache_line create_line(char v, unsigned long long int t, unsigned long long int l)
{
	cache_line c;
	c.vbit = v;
	c.tag = t;
	c.lru = l;
	return c;
}

typedef cache_line* cache_pointer;

typedef cache_pointer* cache_pointer2;

cache_pointer2 cache;

int opt, s, b;

int S, B, E;
char* t;

int hits = 0;

int misses = 0;

int evictions = 0;

void init_cache()
{
	cache = (cache_pointer*) malloc(sizeof(cache_line) * S);

	for(int i = 0; i < S; i++)
	{
		cache[i] = (cache_line*) malloc(sizeof(cache_line) * E);

		for(int j = 0; j < E; j++)
		{
			//set values to 0
			cache[i][j].vbit = 0;
			cache[i][j].tag = 0;
			cache[i][j].lru = 0;
		}
		//calc index mask
	}
}

int fill_empty(int index, unsigned addr)
{
	for(int j = 0; j < E; j++)
	{
		if(!cache[index][j].vbit && cache[index][j].tag == 0)
		{
			cache[index][j].vbit = 1;
			cache[index][j].tag = addr;
			cache[index][j].lru = time(NULL);
			return 0;
		}
	}
	return 1;
}

void evict(int index, unsigned addr)
{
	int y = 0;
	int min = cache[0][0].lru;
	for(int j = 0; j < E; j++)
	{
		if(cache[index][j].lru < min)
		{
			y = j;
			min = cache[index][j].lru;
		}
	}
	cache[index][y].vbit = 1;
	cache[index][y].tag = addr;
	cache[index][y].lru = time(NULL);
}

void accessMemory(char id, unsigned addr, int size)
{
	unsigned tag = addr >> (s + b);
	unsigned mask = pow(2, s) - 1;
	unsigned temp = addr >> b;
	unsigned index = temp & mask;

	int hit = 0;
	for(int j = 0; j < E; j++)
	{
		cache_line c = cache[index][j];
		if(c.vbit)
		{
			if(c.tag == tag)
			{
				hits++;
				hit = 1;
			}
		}
	}
	
	if(!hit)
	{
		misses++;
		if(fill_empty(index, tag))
		{
			evictions++;
			evict(index, tag);
		}
	}
}

void scanFile()
{
	FILE * f;
	f = fopen (t, "r");

	char id;
	unsigned addr;
	int size;

	while(fscanf(f, " %c %x,%d", &id, &addr, &size)>0)
	{
		
		if(id == 'L'|| id == 'S'|| id == 'M')
		{
			accessMemory(id, addr, size);
			//printf("%i\n", addr);
			if(id == 'M')
			{
				accessMemory(id, addr, size);
			}
		}
	}
}

int main(int argc, char** argv)
{

    //int opt, s, b;

    
    while(-1 != (opt = getopt(argc, argv, "s:E:b:t:"))) 
    {
	switch(opt)
	{
	    case 's':
		s = atoi(optarg);
		break;
	    case 'E':
		E = atoi(optarg);
		break;
	    case 'b':
		b = atoi(optarg);
		break;
	    case 't':
		t = optarg;
		break;
	}
    }

    S = pow(2, s);
    B = pow(2, b);

    init_cache();
    scanFile();

    /*printf("%i\n", s);
    printf("%i\n", E);
    printf("%i\n", b);
    printf("%s\n", t);*/

    printSummary(hits, misses, evictions);
    return 0;
}
