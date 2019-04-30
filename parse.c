#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
*	String tokenizer function, arguments are:
*	1) work, pointer to an input_line object containing the input file
*	2) output, pointer to a mid_line object's array, can be NULL
*	3) fill, integer defining how much of the output array has been filled
*	4) read, integer defining how many strings of the input have been read
*	5) output_size, specifies current size of output array
*	6) token, a string where a line is saved when the case "label + something" is encountered, otherwise is NULL
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

/**
 * instruction_decoder function, arguments are:
 * 1) work, single mid_line object containing an instruction that has to be managed
 * 2) symbs, pointer to a global array containing all the symbols appearing in the program
 */
 
instruction *instruction_decoder(mid_line work) {
  char *opcode;
	char *symbol;
	char reg1[5], reg2[5], reg3[5];
  instruction *return_value = malloc(sizeof(instruction));
  int op_size = strlen(work.tokens[0]) + 1;
	int symb_size;
  int size, strip_size;
  family fam;

	/**
	 * first we identify the opcode currently on analisys and its family
	 */
	return_value->opcode = malloc(op_size*sizeof(char));
  strcpy(return_value->opcode, work.tokens[0]);
  fam = family_finder(return_value->opcode);

	/**
	 * Then a switch case separates all the different parsing tecnique 
	 * that we will use to work on different opcode's families
	 * Parsing of the arguments is done through the use of sscanf, 
	 * the string containing the arguments is used as input and a specific regex is then applied
	 * Whenever we are faced with an instruction that contains a symbol it is first ignored during sscanf processing
	 * then it's obtained by stripping the correct ammount of char from front and back of arguments string
	 */
    switch (fam) {
		case 0: 
		/**
		 * family type: u
		 * In this case we have an argument string formatted as "register,offset"
		 */
			return_value->type = u;
			sscanf(work.tokens[1], "%[^,]", reg1);
      		symbol = strip_front(work.tokens[1], strlen(reg1) + 1);
			if (isdigit(symbol[0]) && isdigit(symbol[1])) {
				return_value->imm_field.literal = strtol(symbol, NULL, 10);
				return_value->is_literal = true;
			} else if (isdigit(symbol[0]) && symbol[1] == 'x') {
				symbol = strip_front(symbol, 2);
				return_value->imm_field.literal = strtol(symbol, NULL, 16);
				return_value->is_literal = true;
			} else {
				symb_size = strlen(symbol) + 1;
				return_value->imm_field.symb = malloc(symb_size*sizeof(char));
				strcpy(return_value->imm_field.symb, symbol);
				return_value->is_literal = false;
			}
			return_value->r1 = register_finder(reg1);
			break;
		case 1: 
		/**
		 * family type: i
		 * In this case we have an argument string formatted as "register,register,immediate"
		 */
			return_value->type = i;
			sscanf(work.tokens[1], "%[^,],%[^,]", reg1, reg2);
      		symbol = strip_front(work.tokens[1], strlen(reg1) + strlen(reg2) + 2);
			if (isdigit(symbol[0]) && isdigit(symbol[1])) {
				return_value->imm_field.literal = strtol(symbol, NULL, 10);
				return_value->is_literal = true;
			} else if (isdigit(symbol[0]) && symbol[1] == 'x') {
				symbol = strip_front(symbol, 2);
				return_value->imm_field.literal = strtol(symbol, NULL, 16);
				return_value->is_literal = true;
			} else {
				symb_size = strlen(symbol) + 1;
				return_value->imm_field.symb = malloc(symb_size*sizeof(char));
				strcpy(return_value->imm_field.symb, symbol);
				return_value->is_literal = false;
			}
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			break;
		case 2:
		/**
		 * family type: s
		 * In this case we have an argument string formatted as "register,offset(register)"
		 */
			return_value->type = s;
			sscanf(work.tokens[1], "%[^,],%*[^(](%[^)]", reg1, reg2);
			symbol = strip_front(work.tokens[1], strlen(reg1)+1);
			symbol = strip_back(symbol, (strlen(reg2)+2));
			if (isdigit(symbol[0]) && isdigit(symbol[1])) {
				return_value->imm_field.literal = strtol(symbol, NULL, 10);
				return_value->is_literal = true;
			} else if (isdigit(symbol[0]) && symbol[1] == 'x') {
				symbol = strip_front(symbol, 2);
				return_value->imm_field.literal = strtol(symbol, NULL, 16);
				return_value->is_literal = true;
			} else {
				symb_size = strlen(symbol) + 1;
				return_value->imm_field.symb = malloc(symb_size*sizeof(char));
				strcpy(return_value->imm_field.symb, symbol);
				return_value->is_literal = false;
			}
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			break;
		case 3: 
		/**
		 * family type: r
		 * In this case we have an argument string formatted as "register,register,register"
		 */
			return_value->is_literal = false;
			return_value->type = r;
			sscanf(work.tokens[1], "%[^,],%[^,],%[^,]", reg1, reg2, reg3);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->r3 = register_finder(reg3);
			break;
		case 4: 
		/**
		 * family type: j
		 * In this case we have an argument string formatted as "symbol"
		 * Because of the presence of a symbol we need to set the return_value symbol pointer to 
		 * the correct symbol in our global symbol table, first we check if it's present, then we set the pointer, otherwise 
		 * a new entry is created and added to the table
		 * Note, this family contains j and jal opcodes
		 */
			return_value->type = j;
      		symbol = malloc(strlen(work.tokens[1])*sizeof(char));
			sscanf(work.tokens[1], "%s", symbol);
			if (isdigit(symbol[0]) && isdigit(symbol[1])) {
				return_value->imm_field.literal = strtol(symbol, NULL, 10);
				return_value->is_literal = true;
			} else if (isdigit(symbol[0]) && symbol[1] == 'x') {
				symbol = strip_front(symbol, 2);
				return_value->imm_field.literal = strtol(symbol, NULL, 16);
				return_value->is_literal = true;
			} else {
				symb_size = strlen(symbol) + 1;
				return_value->imm_field.symb = malloc(symb_size*sizeof(char));
				strcpy(return_value->imm_field.symb, symbol);
				return_value->is_literal = false;
			}
			break;
		case 5: 
		/**
		 * family type jr
		 * In this case we have an argument string formatted as "register)"
		 */
		// TODO what's the difference between jr instruction and s instruction? 
			return_value->is_literal = false;
			return_value->type = jr;
			sscanf(work.tokens[1],"%s", reg1);
			return_value->r1 = register_finder(reg1);
			break;
		case 6: 
		/**
		 * family type: b
		 * In this case we have an argument string formatted as "register,register,symbol"
		 */
			return_value->type = b;
			sscanf(work.tokens[1],"%[^,],%[^,]", reg1, reg2);
      		symbol = strip_front(work.tokens[1], strlen(reg1) + strlen(reg2) + 2);
			if (isdigit(symbol[0]) && isdigit(symbol[1])) {
				return_value->imm_field.literal = strtol(symbol, NULL, 10);
				return_value->is_literal = true;
			} else if (isdigit(symbol[0]) && symbol[1] == 'x') {
				symbol = strip_front(symbol, 2);
				return_value->imm_field.literal = strtol(symbol, NULL, 16);
				return_value->is_literal = true;
			} else {
				symb_size = strlen(symbol) + 1;
				return_value->imm_field.symb = malloc(symb_size*sizeof(char));
				strcpy(return_value->imm_field.symb, symbol);
				return_value->is_literal = false;
			}
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			break;
		case 7:
		/**
		 * family type: al
		 * In this case we have an argument string formatted as "register,offset(register)"
		 */
		// TODO what's the difference between al instruction and s instruction? 
			return_value->type = al;
			sscanf(work.tokens[1],"%[^,],%*[^(](%[^)]", reg1, reg2);
			symbol = strip_front(work.tokens[1], strlen(reg1)+1);
			symbol = strip_back(symbol, (strlen(reg2)+2));
			if (isdigit(symbol[0]) && isdigit(symbol[1])) {
				return_value->imm_field.literal = strtol(symbol, NULL, 10);
				return_value->is_literal = true;
			} else if (isdigit(symbol[0]) && symbol[1] == 'x') {
				symbol = strip_front(symbol, 2);
				return_value->imm_field.literal = strtol(symbol, NULL, 16);
				return_value->is_literal = true;
			} else {
				symb_size = strlen(symbol) + 1;
				return_value->imm_field.symb = malloc(symb_size*sizeof(char));
				strcpy(return_value->imm_field.symb, symbol);
				return_value->is_literal = false;
			}
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			break;
		case 8: 
		/**
		 * family type: as
		 * In this case we have an argument string formatted as "register,register,offset(register)"
		 */
			return_value->type = as;
			sscanf(work.tokens[1],"%[^,],%[^,],%*[^(](%[^)]", reg1, reg2, reg3);
			symbol = strip_front(work.tokens[1], strlen(reg1)+strlen(reg2)+2);
			symbol = strip_back(symbol, (strlen(reg2)+2));
			if (isdigit(symbol[0]) && isdigit(symbol[1])) {
				return_value->imm_field.literal = strtol(symbol, NULL, 10);
				return_value->is_literal = true;
			} else if (isdigit(symbol[0]) && symbol[1] == 'x') {
				symbol = strip_front(symbol, 2);
				return_value->imm_field.literal = strtol(symbol, NULL, 16);
				return_value->is_literal = true;
			} else {
				symb_size = strlen(symbol) + 1;
				return_value->imm_field.symb = malloc(symb_size*sizeof(char));
				strcpy(return_value->imm_field.symb, symbol);
				return_value->is_literal = false;
			}
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->r3 = register_finder(reg3);
			break;
		case 9: 
		/**
		 * family type: sext
		 * In this case we have an argument string formatted as "register,register"
		 * Note that this is a pseudo-instruction expanding to "addi reg1,reg2,0"
		 */
			return_value->is_literal = true;
			return_value->type = i;
			sscanf(work.tokens[1],"%[^,],%[^,]", reg1, reg2);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->imm_field.literal = 0;
			break;
		case 10: 
		/**
		 * family type; err
		 * This case handles an incorrect input
		 */
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
