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
String tokenizer function, arguments are:
1) work, pointer to an input_line object containing the input file
2) output, pointer to a mid_line object's array, can be NULL
3) fill, integer defining how much of the output array has been filled
4) read, integer defining how many strings of the input have been read
5) token, a string where a line is saved when the case "label + something" is encountered, otherwise is NULL
*/
mid_line *string_tokenizer(input_lines *work, mid_line *output, int fill, int read, char *token) {
	mid_line *return_value = output;
	int size = work->linecount;
	int token_size;
	int return_token_size = 5;
	int token_count;
	int return_count;
	char *curr_tok;
	char *work_str;
	char *work_tok;
	int tok_size;

	//if return value is NULL it's allocated
	if (return_value == NULL) {
		return_value = malloc(size*sizeof(mid_line));
		return_count = 0;
	} else {
		return_count = fill;
	}

	//for loop starting from the last line that was read
	for (int i = read; i < size; i++) {
		return_token_size = 5;
		token_count = 0;

		//if token is NULL then we simply read our working line from the input structure, otherwise the string that has to be parsed
		//has to be copied from token.
		//work_str must be used because strtok modifies its input and we don't want to ruin ours
		if (token == NULL) {
			tok_size = strlen(work->lines[i]);
			work_str = malloc(tok_size*sizeof(char));
			strcpy(work_str, work->lines[i]);
			curr_tok = strtok(work_str, " ");
			token_size = strlen(curr_tok);
			return_value[return_count].tokens = malloc(return_token_size*sizeof(char*));
		} else {		
			tok_size = strlen(token);
			work_str = malloc(tok_size*sizeof(char));
			strcpy(work_str, token);
			curr_tok = strtok(work_str, " ");
			token_size = strlen(curr_tok);
			return_value[return_count].tokens = malloc(return_token_size*sizeof(char*));
			token = NULL;
		}
		
		//if our token last char is a colon then we alreadt know that it is going to be a label, we can proceed and copy it
		if (curr_tok[token_size-1] == ':') {
			return_value[return_count].role = LABEL;
			//we make sure to have enough space and then use strcpy to copy the token inside our return struct
			return_value[return_count].tokens[token_count] = malloc((strlen(curr_tok)+1)*sizeof(char));
			strcpy(return_value[return_count].tokens[token_count], curr_tok);
			return_value[return_count].token_num = 1;
			curr_tok = strtok(NULL, " ");
			/*if curr_tok is not NULL it means that another line is following, recursion will be used
			first we create a string from all the remaining tokens, then return_value size is increased by one
			and then string_tokenizer is called, with the following arguments:
			work -> work, input has not changed
			output -> return_value, output has not changed either
			fill-> return_count+1 because a new line has been added 
			read-> i because we are still reading the same input line
			token-> remaining_tok, string composed of all the remaining tokens found on the line*/
			if (curr_tok != NULL) {
				char *remaining_tok = NULL;
				tok_size = strlen(curr_tok);
				remaining_tok = malloc((tok_size+1)*sizeof(char));
				strcpy(remaining_tok, curr_tok);
				curr_tok = strtok(NULL, " ");
				while (curr_tok != NULL) {
					tok_size = tok_size + strlen(curr_tok);
					remaining_tok = realloc(remaining_tok, (tok_size+1)*sizeof(char));
					remaining_tok = strcat(remaining_tok, " ");
					remaining_tok = strcat(remaining_tok, curr_tok);
					curr_tok = strtok(NULL, " ");	
				}
				return_value = realloc(return_value, (size+1)*sizeof(mid_line));
				return_value = string_tokenizer(work, return_value, return_count+1, i, remaining_tok);
			}
		//if it's not a label and the first char is a dot then we have a directive, same as before we copy it and check if it has arguments
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
