#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//include parse library function headers and data structures
#include "parse.h"

typedef struct {
	char **tokens;
	int token_num;
	roles role;
}mid_line;

typedef struct {
	char **lines;
	int linecount;
}input_lines;

int isTokenDelim(char value){
    if ((value > TOKEN_A_MAIUSC && value < TOKEN_Z_MAIUSC) || 
        (value > TOKEN_A_MINUSC && value < TOKEN_Z_MINUSC) || 
        (value > TOKEN_ZERO && value < TOKEN_NINE)){
            return(0);
        } else if (value == TOKEN_DOT){
            return(1);
        } else if (value == TOKEN_DOLLAR){
            return(2);
        } else if (value == TOKEN_UNDERSCORE){
            return(3);
        } else if (value == TOKEN_BACKSLASH){
            return(4);
        } else if (value == TOKEN_SHARP){
            return(5);
        } else if (value == TOKEN_DOUBLE_QUOTE){
            return(6);
        } else if (value == TOKEN_SLASH){
            return(7);
        } else if (value == TOKEN_STAR){
            return(8);
        }
    return(-1);
}

char **line_feeder(FILE *work) {
	
}

/*
string tokenizer function, input must be a not NULL input_line element containing a valid assembly file
*/
mid_line *string_tokenizer(input_lines *work) {
	
	if (work == NULL) {
		return NULL;
	}

	mid_line *return_value;
	int size = work->linecount;
	int token_size;
	int return_token_size = 5;
	int token_count;
	int return_count = 0;
	char *curr_tok;
	char *work_str;
	char *work_tok;
	int tok_size;

	return_value = malloc(size*sizeof(mid_line));
	
	for (int i = 0; i < size; i++) {
		return_token_size = 5;
		token_count = 0;

		/*
		first we find the length of the next line that will be analized, then the same line is copied in a different
		variable because strtok modifies its input and we don't want to ruin ours.
		Then we find the first token and begin the analisys
		*/
		tok_size = strlen(work->lines[i]);
		work_str = malloc(tok_size*sizeof(char));
		strcpy(work_str, work->lines[i]);
		curr_tok = strtok(work_str, " ");
		token_size = strlen(curr_tok);
		return_value[return_count].tokens = malloc(return_token_size*sizeof(char*));
		
		//if our token last char is a colon then we alreadt know that it is going to be a label, we can proceed and copy it
		if (curr_tok[token_size-1] == ':') {
			return_value[return_count].role = LABEL;
			//we make sure to have enough space and then use strcpy to copy the token inside our return struct
			return_value[return_count].tokens[token_count] = malloc((strlen(curr_tok)+1)*sizeof(char));
			strcpy(return_value[return_count].tokens[token_count], curr_tok);
			return_value[return_count].token_num = 1;
			curr_tok = strtok(NULL, " ");
			//it's possible to have a label and something else on the same line, here we check if it's happening
			if (curr_tok != NULL) {
				return_count++;
				return_value = realloc(return_value, (size+1)*sizeof(mid_line));
				return_value[return_count].tokens = malloc(return_token_size*sizeof(char*));
				token_size = strlen(curr_tok);
				
				if (curr_tok[token_size-1] == ':') {
					return_value[return_count].role = LABEL;
					return_value[return_count].tokens[token_count] = malloc((strlen(curr_tok)+1)*sizeof(char));
					strcpy(return_value[return_count].tokens[token_count], curr_tok);
					return_value[return_count].token_num = 1;
				} else if (curr_tok[0] == '.') {
					return_value[return_count].role = DIRECTIVE;
					return_value[return_count].tokens[token_count] = malloc((strlen(curr_tok)+1)*sizeof(char));
					strcpy(return_value[return_count].tokens[token_count], curr_tok);
					token_count++;
					curr_tok = strtok(NULL, " ");
					while (curr_tok != NULL) {
						return_value[return_count].tokens[token_count] = malloc((strlen(curr_tok)+1)*sizeof(char));
						strcpy(return_value[return_count].tokens[token_count], curr_tok);
						token_count++;
						if (token_count == return_token_size) {
							return_token_size += 2;
							return_value[return_count].tokens = realloc(return_value[return_count].tokens, return_token_size*sizeof(char*));
						}
						curr_tok = strtok(NULL, " ");
					}
					return_value[return_count].token_num = token_count;
				} else if ((curr_tok[0] >= TOKEN_A_MINUSC) && (curr_tok[size-1] <= TOKEN_Z_MINUSC)) {
					return_value[return_count].role = INSTRUCTION;
					return_value[return_count].token_num = 2;
					return_value[return_count].tokens[token_count] = malloc((strlen(curr_tok)+1)*sizeof(char));
					strcpy(return_value[return_count].tokens[token_count], curr_tok);
					token_count++;
					curr_tok = strtok(NULL, " ");
					return_value[return_count].tokens[token_count] = malloc((strlen(curr_tok)+1)*sizeof(char));
					strcpy(return_value[return_count].tokens[token_count], curr_tok);
				}
			}
		//if it's not a label but the first char is a dot then we have a directive, same as before we copy it and check if it has arguments
		} else if (curr_tok[0] == '.') {
			return_value[return_count].role = DIRECTIVE;
			return_value[return_count].tokens[token_count] = malloc((strlen(curr_tok)+1)*sizeof(char));
			strcpy(return_value[return_count].tokens[token_count], curr_tok);
			token_count++;
			curr_tok = strtok(NULL, " ");
			while (curr_tok != NULL) {
				return_value[return_count].tokens[token_count] = malloc((strlen(curr_tok)+1)*sizeof(char));
				strcpy(return_value[return_count].tokens[token_count], curr_tok);
				token_count++;
				if (token_count == return_token_size) {
					return_token_size += 2;
					return_value[return_count].tokens = realloc(return_value[return_count].tokens, return_token_size*sizeof(char*));
				}
				curr_tok = strtok(NULL, " ");
			}
			return_value[return_count].token_num = token_count;
		//lastly if it's not a label nor a directive we check if it's an instruction.
		//a line containing an instruction will be divided in two different tokens, one containing the opcode
		//and the other containing the arguments, further analysis it's not allowed here
		} else if ((curr_tok[0] >= TOKEN_A_MINUSC) && (curr_tok[size-1] <= TOKEN_Z_MINUSC)) {
			return_value[return_count].role = INSTRUCTION;
			return_value[return_count].token_num = 2;
			return_value[return_count].tokens[token_count] = malloc((strlen(curr_tok) + 1)*sizeof(char));
			strcpy(return_value[return_count].tokens[token_count], curr_tok);
			token_count++;
			curr_tok = strtok(NULL, " ");
			return_value[return_count].tokens[token_count] = malloc((strlen(curr_tok)+1)*sizeof(char));
			strcpy(return_value[return_count].tokens[token_count], curr_tok);
		} else {
			//if none of the previous cases is valid then a non valid input is encountered.
			//a simple error message is printed and a NULL value is returned
			printf("Input value is not valid, line %d, contains unexpected character", i);
			return NULL;
		}
		return_count++;
		//we make sure to free our temporary work string
		free(work_str);
	}
	return return_value;
}

symbol *symbol_decoder(mid_line work) {

}

instruction *instruction_decoder(mid_line work) {

}

directive *directive_decoder(mid_line work) {

}

line *parse(FILE *work){
	
}
