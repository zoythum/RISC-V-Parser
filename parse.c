#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//include parse library function headers and data structures
#include "parse.h"

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


line *parse(FILE *work){
	line *parsed_lines = NULL;
	line *curr_line = NULL;

	unsigned char curr_char;

	//Try allocating the initial line structure...
	curr_line = malloc(sizeof(line));
	//... if allocation failed, set ENOMEM and return immediately
	if (curr_line == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	//Read first character to feel the ground
	curr_char = fgetc(work);
	if (curr_char == EOF){
		//If an error has occurred, throw it and exit...
		if (ferror(work)){
			errno = EIO;
			free(curr_line);
			return NULL;
		}

		//... otherwise, we received an empty file
		curr_line->tokens = NULL;
		curr_line->line = 0;
		curr_line->token_num = 0;

		parsed_lines = curr_line;
	}
	else{
		//WIP: for now, we cleanup and return NULL
		free(curr_line);
	}

	return parsed_lines;
}
