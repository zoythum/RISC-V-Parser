#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//include parse library function headers and data structures
#include "parse.h"

typedef struct {
	char **tokens;
	roles role;
}mid_line;

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

mid_line *string_tokenizer(char **work) {

}

symbol *symbol_decoder(mid_line work) {
    symbol activesymbol = malloc(sizeof(symbol));
    int i = 0;
    char *setbool = null;

    setbool = strchr(*(work.tokens), ':');
    if (setbool == null){
        activesymbol.islab = false;
    } else{
        activesymbol.islab = true;
    }
    while (*(work.tokens)[i] != ' '){
        i++;
    }
    i--;
    strncpy(activesymbol.name, *(work.tokens), i);
    do{
        i++;
    }while (*(work.tokens)[i] == ' ' || *(work.tokens)[i] == '=');
    if (*(work.tokens)[i] == '\0'){
        activesymbol.value = null;
    } else{
        activesymbol.value = atoi(*(work.tokens) + i);
    }
    return &activesymbol;
}

instruction *instruction_decoder(mid_line work) {

}

directive *directive_decoder(mid_line work) {

}

line *parse(FILE *work){
	
}


/*line *parsed_lines = NULL;

	char curr_char;
	char *buff;
	int buff_size = BUFFER_SIZE;
	int buff_count = 0;
	int line_count = 0;


	//Read first character to feel the ground
	curr_char = fgetc(work);
	if (curr_char == EOF){
		//If an error has occurred, throw it and exit...
		if (ferror(work)){
			errno = EIO;
			return NULL;
		}

		//... otherwise, we received an empty file.
		//Try allocating the single returned line structure...
		parsed_lines = malloc(sizeof(line));
		//... if allocation failed, set ENOMEM and return immediately
		if (parsed_lines == NULL) {
			errno = ENOMEM;
			return NULL;
		}

		parsed_lines->tokens = NULL;
		parsed_lines->line = 0;
		parsed_lines->token_num = 0;
	}
	else{
		//Try allocating array of lines and buffer
		parsed_lines = malloc(ARRAY_SIZE*sizeof(line));
		buff = malloc(BUFFER_SIZE*sizeof(char));
		//if isTokenDelim return value is less than 5 we need to read the whole first token
		//to define a line role. Otherwise if is greater or equal than 5 we already know the line role
		while (curr_char != EOF) {
			//buffer is resetted at each line
			buff = malloc(BUFFER_SIZE*sizeof(char));
			buff_count = 0;

			if (isTokenDelim(curr_char) < 5) {
				// TODO: read whole first token to define line's role
			} else {
				switch (isTokenDelim(curr_char)){
					case 5:
						//we will read a sharp directive, we just need to copy it
						while (curr_char != '\n') {
							buff[buff_count] = curr_char;
							buff_count++;
							if (buff_count == BUFFER_SIZE) {
								buff_size += 50;
								buff = realloc(buff, buff_size*sizeof(char));
							}
							curr_char = fgetc(work);
						}
						parsed_lines[line_count].tokens = malloc(sizeof(char*));
						parsed_lines[line_count].line = line_count;
						parsed_lines[line_count].role = COMMENT;
						parsed_lines[line_count].token_num = 1;
						strcpy(parsed_lines[line_count].tokens[0], buff);
						line_count++;
					case 6:
						//having read a double quote we expect to find another double
						//quote followed by a colon. Between double quotes all character are possible
						//HOW TO: it's possible to find multiple tokens in a line where the first one 
						//is a label, we decided to split this "mixed" line into multiple, mono function
						//lines, due to this decision we read only untill we find a double quote followed 
						//by a colon

						buff[buff_count] = curr_char;
						curr_char = fgetc(work);
						//this particular while-loop is managed in a different way, for all double quote
						//read we check if the previous char was a backslash because that would allow the
						//presence of a double quote inside the string, otherwise we jump out the loop
						while (1) {
							buff[buff_count] = curr_char;
							buff_count++;
							if (buff_count == BUFFER_SIZE) {
								buff_size += 50;
								buff = realloc(buff, buff_size*sizeof(char));
							}
							curr_char = fgetc(work);
							if (isTokenDelim(curr_char) == 6) {
								if (isTokenDelim(buff[buff_count-1] != 5)) {
									break;
								}
							}
						}
						buff[buff_count] = curr_char;
						curr_char = fgetc(work);
						if (curr_char != ':') {
							// TODO: raise error, only a colon can be found after a double quoted string
						}
						parsed_lines[line_count].tokens = malloc(sizeof(char*));
						parsed_lines[line_count].line = line_count;
						parsed_lines[line_count].role = LABEL;
						parsed_lines[line_count].token_num = 1;
						strcpy(parsed_lines[line_count].tokens[0], buff);
						line_count++;
					case 7:
						// TODO: read comment and ignore it
					default:
						// TODO: raise error
						return 0;
				}

			}
		}
	}

	return parsed_lines;*/