#include <stdio.h>
#include <unistd.h>
#include "c-sim.h"
#include <math.h>



int main(int argc, char *argv[])
{
	if(argc < 2){
        printf("error\n");
        return 1;
	}

	int cache_size = atoi(argv[1]);
	char* associativity = argv[2];
	int block_size = atoi(argv[3]);
	char* replacement_policy = argv[4];
	char* write_policy = argv[5];
	char* trace_file = argv[6];


	initialize(cache_size, block_size, replacement_policy, associativity);



    //printf("number of index:%d\n", numberOfIndex);
    //printf("number_set:%d\n", (int)pow(2,numberOfIndex));
	open_file(trace_file, write_policy, replacement_policy);


    printf("Memory reads:%d\nMemory writes:%d\nCache hits:%d\nCache misses:%d\n", number_read, number_write, number_hit, number_miss);
//	replace_cache(replacement_policy);
//	write_cache(write_policy);
//	printf("cache size:%d  assoc:%s  block size:%d  replace:%s  write:%s  file name:%s\n",cache_size,associativity,block_size,replacement_policy,write_policy,trace_file);

    return 0;

}
