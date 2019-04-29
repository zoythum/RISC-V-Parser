#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//include parse library function headers and data structures
#include "parse.h"
#include "utils.h"

typedef struct {
	char **tokens;
	int token_num;
	roles role;
}mid_line;

typedef struct {
	char **lines;
	int linecount;
}input_lines;

char **line_feeder(FILE *work) {
	
}
/*
String tokenizer function, arguments are:
1) work, pointer to an input_line object containing the input file
2) output, pointer to a mid_line object's array, can be NULL
3) fill, integer defining how much of the output array has been filled
4) read, integer defining how many strings of the input have been read
5) output_size, specifies current size of output array
6) token, a string where a line is saved when the case "label + something" is encountered, otherwise is NULL
*/
mid_line *string_tokenizer(input_lines *work, mid_line *output, int fill, int read, int output_size, char *token) {
	mid_line *return_value = output;
	int size;
	int token_size;
	int return_token_size = 5;
	int token_count;
	int return_count;
	int tok_size;
	char *curr_tok;
	char *work_str;
	char *work_tok;
	char *remaining_tok = NULL;

	//if return value is NULL it's allocated
	if (return_value == NULL) {
		size = work->linecount;
		return_value = malloc(size*sizeof(mid_line));
		return_count = 0;
	} else {
		size = output_size;
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
			output_size-> size, current allocated size of our output
			token-> remaining_tok, string composed of all the remaining tokens found on the line*/
			if (curr_tok != NULL) {
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
				return_value = string_tokenizer(work, return_value, return_count+1, i, size, remaining_tok);
				return(return_value);
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

instruction *instruction_decoder(mid_line work, symb_tab *symbs) {
    char *opcode;
	char *symbol;
	char reg1[5], reg2[5], reg3[5];
	symb_tab *ptr;
    instruction *return_value = malloc(sizeof(instruction));
    int op_size = strlen(work.tokens[0]) + 1;
    family fam;

    opcode = malloc(op_size*sizeof(char));
    strcpy(opcode, work.tokens[0]);
    fam = family_finder(opcode);

    switch (fam) {
		case 0: //family type: u
			return_value->is_literal = true;
			return_value->type = u;
			sscanf(work.tokens[1], "%[^,],%d", reg1, &return_value->imm_field.literal);
			return_value->r1 = register_finder(reg1);
			break;
		case 1: //family type: i
			return_value->is_literal = true;
			return_value->type = i;
			sscanf(work.tokens[1], "%[^,],%[^,],%d", reg1, reg2, &return_value->imm_field.literal);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			break;
		case 2: //family type: s
			return_value->is_literal = true;
			return_value->type = s;
			sscanf(work.tokens[1], "%[^,],%d(%[^)]", reg1, &return_value->imm_field.literal, reg2);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			break;
		case 3: //family type: r
			return_value->is_literal = false;
			return_value->type = r;
			sscanf(work.tokens[1], "%[^,],%[^,],%[^,]", reg1, reg2, reg3);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->r3 = register_finder(reg3);
			break;
		case 4: //family type: j
			return_value->is_literal = true;
			return_value->type = j;
			sscanf(work.tokens[1], "%[^,],%s", reg1, symbol);
			return_value->r1 = register_finder(reg1);

			ptr = symbs;
			while ((strcmp(ptr->sym->name, symbol) != 0) || (ptr != NULL)) {
				ptr = ptr->next;
			}
			if (ptr == NULL) {
				// TODO check if this really works
				mid_line temp;
				int symb_size = strlen(symbol)+1;
				temp.tokens = malloc(sizeof(*char));
				temp.tokens[0] = malloc(symb_size*sizeof(char));
				strcpy(temp.tokens[0], symbol);
				temp.role = LABEL;
				temp.token_num = 1;
				ptr->next = malloc(sizeof(symb_tab));
				ptr->next->prev = ptr;
				ptr = ptr->next;
				ptr->next = NULL;
				ptr->sym = symbol_decoder(temp);
			}
			return_value->imm_field.symb = ptr->sym;
			break;
		case 5: //family type ja
			return_value->is_literal = true;
			return_value->type = ja;
			sscanf(work.tokens[1],"%[^,],%d(%[^)]", reg1, &return_value->imm_field.literal, reg2);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			break;
		case 6: //family type jr

		case 7: //family type: b
			return_value->is_literal = true;
			return_value->type = b;
			sscanf(work.tokens[1],"%[^,],%[^,],%s", reg1, reg2, symbol);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			ptr = symbs;
			while ((strcmp(ptr->sym->name, symbol) != 0) || (ptr != NULL)) {
				ptr = ptr->next;
			}
			if (ptr == NULL) {
				// TODO check if this really works
				mid_line temp;
				int symb_size = strlen(symbol)+1;
				temp.tokens = malloc(sizeof(*char));
				temp.tokens[0] = malloc(symb_size*sizeof(char));
				strcpy(temp.tokens[0], symbol);
				temp.role = LABEL;
				temp.token_num = 1;
				ptr->next = malloc(sizeof(symb_tab));
				ptr->next->prev = ptr;
				ptr = ptr->next;
				ptr->next = NULL;
				ptr->sym = symbol_decoder(temp);
			}
			return_value->imm_field.symb = ptr->sym;
			break;
		case 8: //family type: al
			return_value->is_literal = true;
			return_value->type = al;
			sscanf(work.tokens[1],"%[^,],%d(%[^)]", reg1, &return_value->imm_field.literal, reg2);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			break;
		case 9: //family type: as
			return_value->is_literal = true;
			return_value->type = as;
			sscanf(work.tokens[1],"%[^,],%[^,],%d(%[^)]", reg1, reg2, &return_value->imm_field.literal, reg3);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->r3 = register_finder(reg3);
			break;
		case 10: //family type: sext
			return_value->is_literal = true;
			return_value->type = i;
			sscanf(work.tokens[1],"%[^,],%[^,]", reg1, reg2);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->imm_field.literal = 0;
			break;
		case 11: //family type; err
			printf("input is not correct");
			return NULL;
		default:
			break;
    }
    return (return_value);
}

directive *directive_decoder(mid_line work) {

}

line *parse(FILE *work){
	
}
