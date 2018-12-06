#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "help.h"
#include <math.h>

int numberOfIndex = 0;
int offset_len = 0;
int numberOfLine = 0; // total number of lines
int numberofLineInEachSet = 0;

int number_read = 0;
int number_write = 0;
int number_hit = 0;
int number_miss = 0;


int IsPowerOfTwo(int x){
    return (x > 0) && ((x & (x - 1)) == 0);
}


void printCache(){
    address_info *curr = NULL;
    for(int i = 0; i<numberOfLine/numberofLineInEachSet; i++){
        printf("Set: %d\n", i);
        curr = cache[i];

        if(curr == NULL) continue;
        while(curr->next != NULL){
            printf("\tTag: %d\n", curr->tag);
            printf("\tValid: %d\n", curr->valid);
            printf("\tDirty Bit: %d\n\n", curr->dirty);
            curr = curr->next;
        }
        printf("\tTag: %d\n", curr->tag);
        printf("\tValid: %d\n", curr->valid);
        printf("\tDirty Bit: %d\n", curr->dirty);
    }
    	printf("hit:%d  miss:%d  read:%d    write:%d\n------------------------------\n", number_hit, number_miss, number_read, number_write);
}

//set up the number of lines, number of index, offset length
int initialize(int cache_size, int block_size, char* replacement_policy, char*associativity){
        //int count = 1;

        numberOfLine = cache_size/block_size; // total number of lines

        offset_len = log2(block_size);

        if(!IsPowerOfTwo(cache_size)){
            printf("Cache size is not a power of two.\n");
            return 1;
        }

        if(strcmp(associativity, "direct") == 0){
            numberOfIndex = log2(numberOfLine);

        }else if(strcmp(associativity, "assoc") == 0){
            numberOfIndex = 0;

        }else if(strncmp(associativity, "assoc:", 6) == 0){
                char* temp = associativity+6;
                int num = atoi(temp);
                numberOfIndex = log2(numberOfLine / num);

                if(!IsPowerOfTwo(num) || num > cache_size){
                    printf("Input error on associativity.\n");
                    return 1;
                }

        }else{
                printf("Input error on associativity.\n");
                return 1;
        }

        //printf("offset:%d  numberoflines:%d  numberofIndex:%d\n",offset_len, numberOfLine, numberOfIndex);

        numberofLineInEachSet = numberOfLine/ (int)pow(2,numberOfIndex);

        //printf("number of lines in each set:%d", numberofLineInEachSet);


        //todo:should i use ** or *???
        cache = (address_info**)malloc(sizeof(address_info*) * numberOfLine / numberofLineInEachSet);


//        printf("Size of address info:%d\n", (int)sizeof(address_info));
//        printf("size malloc:%d\n", (int)(sizeof(address_info) * (int)pow(2,numberOfIndex) * numberofLineInEachSet));
//        printf("number of lines in each set:%d\n", numberofLineInEachSet);
        return 0;
}


void write_cache(int setindex, int tag, char* write_policy, char* replacement_policy, address_info* node){
    int miss_hit = 0;// miss:0,hit:1
    int numberOfExisting = 0;

    struct address_info* pt = cache[setindex];

    node -> dirty = 1;//whenever write, dirty bit is always 1

    if(cache[setindex] == NULL){//when the set is empty
        cache[setindex] = node;
        number_read++;
        number_miss++;

        if(strcmp(write_policy,"wt") == 0){
            number_write++;
//        }else if(strcmp(write_policy,"wb") == 0){
//            node -> dirty = 1;
        }
        return;
    }


    if(strcmp(write_policy,"wt") == 0){
        while(pt != NULL){
            numberOfExisting++;
            if(pt->tag == tag){
                miss_hit = 1;
                pt->dirty = 1;
            }
            pt = pt -> next;

        }
        if(miss_hit == 0){// if it is a miss for wt
            if(numberOfExisting == numberofLineInEachSet){
                removeLastNode(cache[setindex],setindex);
            }
            node -> next = cache[setindex];
            cache[setindex] = node;
            number_miss++;
            number_read++;
            number_write++;
        }else{
            number_hit++;
            number_write++;

            if(strcmp(replacement_policy,"FIFO") == 0){
                free(node);
            }else if(strcmp(replacement_policy,"LRU") == 0){
                removeTaggedNode(cache[setindex],tag,setindex);
                node -> next = cache[setindex];
                cache[setindex] = node;
            }
        }
    }else if(strcmp(write_policy,"wb")==0){
        address_info* temp = NULL;

        while(pt != NULL){
            numberOfExisting++;
            if(pt->tag == tag){
                miss_hit = 1;
                temp = pt;
            }
            pt = pt -> next;

        }
        if(miss_hit == 0){// if it is a miss for wb
            if(numberOfExisting == numberofLineInEachSet){//set is full
                if(removeLastNode(cache[setindex],setindex) == 1){
                    number_write++;
                }
            }
            node -> next = cache[setindex];
            cache[setindex] = node;
            number_miss++;
            number_read++;
        }else{
            number_hit++;

            if(strcmp(replacement_policy,"FIFO") == 0){
                free(node);

                if(temp -> dirty == 1){
                    number_write++;
                }else{
                    temp -> dirty = 1;
                }

                //already marked the dirty bit 1

            }else if(strcmp(replacement_policy,"LRU") == 0){
                if(removeTaggedNode(cache[setindex],tag,setindex) == 1){
                    number_write++;
                }
                node -> next = cache[setindex];
                cache[setindex] = node;
            }
        }
    }
}

void read_cache(int setindex, int tag, char* write_policy, char* replacement_policy, address_info* node){

    int miss_hit = 0;// miss:0,hit:1
    int numberOfExisting = 0;

    struct address_info* pt = cache[setindex];

    if(cache[setindex] == NULL){//when the set is empty
        cache[setindex] = node;
        number_read++;
        number_miss++;
        return;
    }

    if(strcmp(write_policy,"wt")==0){
        while(pt != NULL){
            numberOfExisting++;
            if(pt->tag == tag){
                miss_hit = 1;
            }
            pt = pt -> next;

        }
        if(miss_hit == 0){
            if(numberOfExisting == numberofLineInEachSet){
                removeLastNode(cache[setindex],setindex);
            }
            node -> next = cache[setindex];
            cache[setindex] = node;
            number_miss++;
            number_read++;
        }else{
            number_hit++;

            if(strcmp(replacement_policy,"FIFO") == 0){
                free(node);
            }else if(strcmp(replacement_policy,"LRU") == 0){
                removeTaggedNode(cache[setindex],tag,setindex);
                node -> next = cache[setindex];
                cache[setindex] = node;
            }
        }
    }else if(strcmp(write_policy,"wb")==0){
        while(pt != NULL){
            numberOfExisting++;
            if(pt->tag == tag){
                miss_hit = 1;
                break;
            }
            pt = pt -> next;

        }
        if(miss_hit == 0){
            if(numberOfExisting == numberofLineInEachSet){// if it is a miss
                if(removeLastNode(cache[setindex],setindex) == 1){ // if the dirty bit is 1, do I write?
                    number_write++;
                }
            }
            node -> next = cache[setindex];
            cache[setindex] = node;
            number_miss++;
            number_read++;
        }else{
            number_hit++;

            if(strcmp(replacement_policy,"FIFO") == 0){
                if(pt -> dirty == 1) number_write++;
                else pt-> dirty = 1;
                free(node);
            }else if(strcmp(replacement_policy,"LRU") == 0){
                if(removeTaggedNode(cache[setindex],tag,setindex) == 1)
                    number_write++;

                node -> next = cache[setindex];
                cache[setindex] = node;
            }
        }
    }else{
        printf("Input error on write policy.\n");
    }
}


int open_file(char* trace_file, char* write_policy, char* replacement_policy){
	if(access(trace_file, F_OK ) != -1){
        FILE *fp;
        //int num;
        //char c[1];

        struct address_info *memory;

        fp = fopen(trace_file,"r");

        char ip[12], rw[2], address[11];
        while (fscanf(fp, "%s %s %s", ip, rw, address) != EOF && strcmp(ip, "#eof") != 0) {

            char* bin_address = binaryFromHex(address);

            //printf("rw:%s\naddress:%s\n",rw,bin_address);

            bin_address[strlen(bin_address) - offset_len]=0;
            //printf("address:%s\n", bin_address);//print out the changed address(remove offset)

            memory = (address_info *)malloc(sizeof(struct address_info));
            /* update the memory information*/
            initializeAddressInfo(memory);
            if(numberOfIndex != 0){
                char* index = bin_address+(strlen(bin_address) - numberOfIndex);// the index number
                memory ->set_index = binaryToDecimal(index);
                //printf("index:%s   %d\n", index, memory ->set_index);
            }
                bin_address[strlen(bin_address) - numberOfIndex]=0;

                memory ->tag =  binaryToDecimal(bin_address);
                //printf("tag:%s  %d\n",bin_address,memory->tag);


            free(bin_address);//change it later

            //printf("Before: hit:%d  miss:%d  read:%d    write:%d    rw:%s\n------------------------------\n", number_hit, number_miss, number_read, number_write, rw);


            if(strcmp(rw,"R")==0){
                read_cache(memory->set_index, memory->tag, write_policy, replacement_policy, memory);
                //testing
            }else{
                write_cache(memory->set_index, memory->tag, write_policy, replacement_policy, memory);

            }


            //printCache();

        }
        fclose(fp);
    }else{
        printf("File does not exist or cannot open.\n");
        return 1;
    }

	return 0;
}


