#include <stdio.h>
#include "common.h"
//--------------------------------Scanner Begin--------------------------------
#define BUFLEN 80

int row, col, len, pos;
char buffer[BUFLEN];
char lastch;
char cur;
FILE * f;	

char scan(){
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

void init_scan(){
	lastch = -1;
	row = 1;
	col = 0;
	len = 0;
	pos = 0;
}
//--------------------------------Scanner End--------------------------------


//--------------------------------Tokenizer Begin--------------------------------
#define TOKLEN 100
enum Tag tag;
//for const string & id
char tokstr[TOKLEN];
//for const number
int tokval;
//for const char
char tokch;
typedef struct {
	char keyword[10];
	enum Tag tag;
}KTpair;

//for convenience of seperating keyword from identifier
KTpair kt[15] = {
    {"int", kw_int},
    {"char", kw_char},
    {"void", kw_void},
    {"extern", kw_extern},
    {"if", kw_if},
    {"else", kw_else},
    {"switch", kw_switch},
    {"case", kw_case},
    {"default", kw_default},
    {"while", kw_while},
    {"do", kw_do},
    {"for", kw_for},
    {"break", kw_break},
    {"continue", kw_continue},
    {"return", kw_return}};

//la for look ahead
char la;

enum Tag idtag(){
	int i;
	for (i=0;i<15;++i)
		if (strcmp(tokstr, kt[i].keyword)==0)
			return kt[i].tag;
	return id;
}

void scanop(char c, enum Tag t1, enum Tag t2){
	la = scan();
	if (la==c){
		tag = t1;
		la = scan();
	}
	else tag = t2;
}

void tokenize(){
	int i;
	//space
	while (la==' '||la=='\t'||la=='\n')
		la = scan();
	//identifier&keyword
	if (la>='a'&&la<='z'||la>='A'&&la<='Z'||la=='_'){
		i = 0;
		do{
			tokstr[i++] = la;
			la = scan();
		} while (la>='a'&&la<='z'||la>='A'&&la<='Z'||la=='_'||la>='0'&&la<='9');
		tokstr[i] = 0;
		tag = idtag();
		return ;
	}
	//const_num
	if (la>='0'&&la<='9'){
		tokval = 0;
		tag = const_num;
		if (la!='0'){
			//Dec
			do{
				tokval = tokval*10 + la - '0';
				la = scan();
			}while (la>='0'&&la<='9');
		}
		else{
			la = scan();
			if (la=='x'){
				//Hex
				la = scan();
				if (la>='0'&&la<='9'||la>='a'&&la<='f'||la>='A'&&la<='F'){
					do{
						tokval = tokval*16 + la;
						if (la<='9')
							tokval -= '0';
						else if (la<='F')
							tokval -= 'A';
						else if (la<='f')
							tokval -= 'a';
						la = scan();
					}while(la>='0'&&la<='9'||la>='a'&&la<='f'||la>='A'&&la<='F');
				}
				else{
					tag = err;
				}
			}
			else if (la=='b'){
				//Bin
				la = scan();
				if (la>='0'&&la<='1'){
					do{
						tokval = tokval*2 + la - '0';
						la = scan();
					}while(la>='0'&&la<='1');
				}
				else{
					tag = err;
				}
			}
			else if (la>='0'&&la<='7'){
				//Oct
				do{
					tokval = tokval*8 + la - '0';
					la = scan();
				}while(la>='0'&&la<='7');
			}
		}
		return ;
	}
	//const_ch
	if (la=='\''){
		tag = const_ch;
		la = scan();
		if (la=='\\'){
			la = scan();
			if (la=='n')
				tokch = '\n';
			else if (la=='\\')
				tokch = '\\';
			else if (la=='t')
				tokch = '\t';
			else if (la=='\'')
				tokch = '\'';
			else if (la=='0')
				tokch = '\0';
			else if (la==-1||la=='\n')
				tag = err;
			else tokch = la;
		}
		else if (la=='\n'||la==-1)
			// not complete
			tag = err;
		else if (la=='\'')
			// no data
			tag = err;
		else tokch = la;
		if (tag==err)
			return ;
		la = scan();
		if (la!='\'')
			tag = err;
		la = scan();
		return ;
	}
	//const_str
	if (la=='"'){
		i = 0;
		la = scan();
		tag = const_str;
		while (la!='"'){
			if (la=='\\'){
				la = scan();
            	if (la=='n')
                	tokstr[i] = '\n';
            	else if (la=='\\')
                	tokstr[i] = '\\';
            	else if (la=='t')
                	tokstr[i] = '\t';
            	else if (la=='\'')
                	tokstr[i] = '\'';
            	else if (la=='0')
                	tokstr[i] = '\0';
            	else if (la==-1)
                	tag = err;
				else if (la=='\n')
					;//allow string across lines
            	else tokstr[i] = la;
				i++;
			}
			else if (la=='\n'||la==-1){
				tag=err;
				break;
			}
			else tokstr[i++] = la;
			la = scan(); 
		}
		la = scan();
		tokstr[i] = 0;
		return ;
	}
	//op&semi
	switch (la){
		case '+':
			scanop('+', inc, add);
			break;
		case '-':
			scanop('-', dec, sub);
			break;
		case '*':
			tag = mul;
			la = scan();
			break;
		case '/':
			la = scan();
			//deal with comment
			if (la=='/'){
				//ignore until '\n'
				while (la!='\n'&&la!=-1)
					la = scan();
				tag = comment;
			}
			else if (la=='*'){
				//ignore until '*/'
				la = scan();
				while (la!=-1){
					if (la=='*'){
						while (la=='*')
							la = scan();
						if (la=='/')
							break;
					}
				}
				if (la=='/'){
					tag = comment;
					la = scan();
				}
				else tag = err;
			}
			else {
				tag = div;
				la = scan();
			}
			break;
		case '%':
			tag = mod;
			la = scan();
			break;
		case '!':
			tag = bool_not;
			la = scan();
			break;
		case '&':
			scanop('&', bool_and, bit_and);
			break;
		case '|':
			scanop('|', bool_or, bit_or);
			break;
		case '~':
			tag = neg;
			la = scan();
			break;
		case '>':
			scanop('=', ge, gt);
			break;
		case '<':
			scanop('=', le, lt);
			break;
		case '=':
			scanop('=', equ, assign);
		case ':':
			tag = colon;
			la = scan();
			break;
		case ',':
			tag = comma;
			la = scan();
			break;
		case ';':
			tag = semicolon;
			la = scan();
			break;
		case '(':
			tag = lparen;
			la = scan();
			break;
		case ')':
			tag = rparen;
			la = scan();
			break;
		case '{':
			tag = lbrace;
			la = scan();
			break;
		case '}':
			tag = rbrace;
			la = scan();
			break;
		case '[':
			tag = lbrack;
			la = scan();
			break;
		case ']':
			tag = rbrack;
			la = scan();
			break;
		case -1:
			// meet eof, return end to parser to inform it of the final token
			tag = end;
			break;
		default:
			tag = err;
	}
}

void init_tokenize(){
	la = ' ';
}
//--------------------------------Tokenizer End--------------------------------

//--------------------------------Tokenizer Test Begin--------------------------------
int main(){
	f = fopen("../testcase/test.c", "r");
	if (!f){
		printf("open error\n");
		return 0;
	}
	init_scan();
	init_tokenize();
	char c;
	while (1){
		tokenize();
		if (tag == err){
			printf("error occur\n");
			break;
		}
		if (tag == end){
			printf("end tokenizing\n");
			break;
		}
		printf("tag: %s", tagstrs[tag]);
		if (tag==const_num)
			printf(", value = %d", tokval);
		else if (tag==const_str)
			printf(", string = %s", tokstr);
		else if (tag==const_ch)
			printf(", char = %c", tokch);
		else if (tag==id)
			printf(", identifier = %s", tokstr);
		printf("\n");
	}
	return 0;
}
//--------------------------------Tokenizer Test End--------------------------------
