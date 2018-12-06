#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


typedef struct address_info{
    int tag;
    int valid;
    int dirty;
    int set_index;
    struct address_info* next;
}address_info;

address_info** cache;

void initializeAddressInfo(struct address_info *memory) {
    	memory -> tag = -1;
    	memory -> valid = 0;
    	memory -> dirty = 0;
    	memory -> set_index = 0;
    	memory -> next = NULL;
}

int binaryToDecimal(char *binary) {
    	int i;
    	int result = 0;
    	int power = 0;
    	for (i = strlen(binary) - 1; i >= 0; i--) {
    	    	int added = (binary[i] - '0') * (int)pow(2, power);
    	    	result += added;
    	    	power++;
    	}
    	return result;
}


/*Converts a hexadecimal address into binary form*/
char* binaryFromHex(char *address) {
    	int i;
    	char *bin_add = malloc(sizeof(char) * 33);
    	for(i = 2; i < strlen(address); i++) {
        	switch(address[i]) {
                    case '0': strcat(bin_add,"0000"); break;
            		case '1': strcat(bin_add,"0001"); break;
            		case '2': strcat(bin_add,"0010"); break;
            		case '3': strcat(bin_add,"0011"); break;
            		case '4': strcat(bin_add,"0100"); break;
            		case '5': strcat(bin_add,"0101"); break;
            		case '6': strcat(bin_add,"0110"); break;
            		case '7': strcat(bin_add,"0111"); break;
            		case '8': strcat(bin_add,"1000"); break;
            		case '9': strcat(bin_add,"1001"); break;
            		case 'a': strcat(bin_add,"1010"); break;
            		case 'b': strcat(bin_add,"1011"); break;
            		case 'c': strcat(bin_add,"1100"); break;
            		case 'd': strcat(bin_add,"1101"); break;
            		case 'e': strcat(bin_add,"1110"); break;
            		case 'f': strcat(bin_add,"1111"); break;
        	}
    	}

        strcat(bin_add,"\0");
    	return bin_add;
}

/*Gets number of lines in the trace file*/
int getNumLines(FILE *trace_file) {
	int ch, num_lines = 0;
    	do {
		ch = fgetc(trace_file);
    	    	if (ch == '\n')
            		num_lines++;
    	} while (ch != EOF);
    	return num_lines;
}


int removeLastNode(struct address_info* node, int setindex){
    if(node == NULL) return 0;
    if(node -> next == NULL){
        int dirtybit = node -> dirty;
        free(node);
        cache[setindex] = NULL;
        return dirtybit;
    }
    struct address_info* pt = node;
    while(pt -> next -> next != NULL){
        pt = pt -> next;
    }
    int dirtybit = pt -> next -> dirty;
    free(pt -> next);
    pt -> next = NULL;
    return dirtybit;
}


int removeTaggedNode(struct address_info* node, int tag, int setIndex){
    int dirty;
    if(node->tag == tag){
        struct address_info* temp = node -> next;
        dirty = node -> dirty;
        free(node);
        cache[setIndex] = temp;
        return dirty;
    }else if(node->next->tag == tag){
        struct address_info* temp = node -> next;
        dirty = temp -> dirty;
        node -> next = node -> next -> next;
        free(temp);
        return dirty;
    }
    struct address_info* pre = node;
    struct address_info* pt = pre -> next;

    while(pt -> next != NULL){
        dirty = pt -> dirty;
        if(pt->tag == tag){
            pre->next = pre->next->next;
        }
        pre = pre -> next;
        pt = pt -> next;
    }
    return dirty;

}



