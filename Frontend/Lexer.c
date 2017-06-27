#include <stdio.h>
#include "common.h"
//--------------------------------Scanner Begin--------------------------------
#define BUFLEN 80

int row, col, len, pos;
char buffer[BUFLEN];
char lastch;
char cur;	

char scan(FILE * f){
	if (!f)
		return -1;
	//if buffer not empty, read from buffer
	if (pos == len){
		len = fread(buffer, 1, BUFLEN, f);
		if (len == 0){
			fclose(f);
			return -1;
		}
		pos = 0;
	}
	cur = buffer[pos++];
	if (lastch == '\n'){
		row++;
		col = 1;
	}
	else col++;
	lastch = cur;
	return cur;
}

void init(){
	lastch = -1;
	row = 1;
	col = 0;
	len = 0;
	pos = 0;
}
//--------------------------------Scanner End--------------------------------


//--------------------------------Tokenizer Begin--------------------------------

//--------------------------------Tokenizer End--------------------------------
/*
int main(){
	FILE * f = fopen("../README.md", "r");
	if (!f){
		printf("open error\n");
		return 0;
	}
	init();
	char c;
	while (1){
		c = scan(f);
		if (c != -1)
			printf("row: %d, col: %d, char: %c\n", row, col, c);
		else break;
	}
	return 0;
}
*/
