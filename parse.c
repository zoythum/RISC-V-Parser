#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//include parse library function headers and data structures
#include "parse.h"

#define INITIAL_RETURNED_LINES_COLLECTION_SIZE 1000
#define INITIAL_INPUT_LINEBUFFER_SIZE 40

typedef struct {
	char **tokens;
	roles role;
}mid_line;

//Structure containing the input file's meaningful normalized lines and their count.
typedef struct {
	char **lines;
	int linecount;
} input_lines;


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

/*
 * Reads the input stream line-by-line, checking for valid GNU Assembler syntax and normalizing its contents.
 * Returns an input_lines structure.
 *
 * EXTENSIONS USED: GNU getline()
 *
 * THIS FUNCTION MANIPULATES THE errno GLOBAL VARIABLE in the following way:
 * - ENOMEM: when any of the memory allocation functions fails
 * - EIO: when any of the input reading functions fails
 *
 * In case of empty file, returns an input_lines consisting of a single line of a single null character.
 * In case of unrecoverable errors, sets the appropriate errno value (if applicable) and returns an 'errored structure':
 * - lines == NULL
 * - linecount == -1
 */
input_lines line_feeder(FILE *work) {

	//Returned structure
	input_lines accum;

	//Buffer for getline()'s read input
	struct linebuffer {
		char *contents;
		size_t buffer_size;
	} curr_line;

	//Input-reading functions return value
	int retval;

	char curr_char;

	/*Enumeration which specifies failures (or the lack thereof) during various phases of this function's execution:
	 * FAIL_RET_ALLOC: return structure initial allocation failed
	 * FAIL_BUFF_ALLOC: buffer initial allocation failed
	 * FAIL_PARSE: unrecoverable error encountered during parsing
	 * SUCCESS: no errors
	 */
	enum parse_state { FAIL_RET_ALLOC, FAIL_BUFF_ALLOC, FAIL_PARSE, SUCCESS } status;


	/* INIT */

	//Try to initialize the line buffer with the specified initial capacity.
	curr_line.contents = (char*) malloc(INITIAL_INPUT_LINEBUFFER_SIZE * sizeof(char));

	//If allocation failed, we're probably starving on memory; set ENOMEM and return immediately with an errored structure.
	if(curr_line.contents == NULL) {
		status = FAIL_BUFF_ALLOC;
		errno = ENOMEM;
		goto CLNP;
	}

	curr_line.buffer_size = INITIAL_INPUT_LINEBUFFER_SIZE;

	//Try to initialize the return structure's lines array with the specified initial capacity.
	accum.lines = (char**) malloc(INITIAL_RETURNED_LINES_COLLECTION_SIZE * sizeof(char*));

	//If allocation failed, we're probably starving on memory; set ENOMEM and return immediately with an errored structure.
	if(accum.lines == NULL) {
		status = FAIL_RET_ALLOC;
		errno = ENOMEM;
		goto CLNP;
	}

	accum.linecount = 0;



	/* EXEC */

	//Read first line to feel the ground
	retval = getline(&(curr_line.contents), &(curr_line.buffer_size), work);

	//Test for possible errors
	if(retval == -1){

		//If an error has occurred, throw it, construct and return immediately with an errored structure.
		if(ferror(work)) {
			status = FAIL_PARSE;
			errno = EIO;
			goto CLNP;
		}
		else {
			//... otherwise, we received an empty file, which is prefectly fine.
			//Try to allocate a single line;
			accum.lines[0] = (char*) malloc(sizeof(char));

			//If allocation failed, we're probably starving on memory; set ENOMEM and return immediately with an errored structure.
			if(accum.lines[0] == NULL) {
				status = FAIL_PARSE;
				errno = ENOMEM;
				goto CLNP;
			}
			else {
				//All is well; proceed with normal cleanup and return.
				status = SUCCESS;
				accum.lines[0][0] = '\0';
				accum.linecount = 1;
			}
		}
	}
	status = SUCCESS;


	/* CLEANUP */
CLNP:
	{
		char **swinger = accum.lines;

		switch(status) {
			case SUCCESS:
				//Compact the returned structure and free the buffer.
				accum.lines = realloc(accum.lines, accum.linecount * sizeof(char*));

				//If for some reason realloc() fails, proceed as if we errored during parsing.
				if (accum.lines == NULL){
					accum.lines = swinger;
					status = FAIL_PARSE;
					errno = ENOMEM;
					goto CLNP;
				}
				else
					free(curr_line.contents);

				break;
			case FAIL_PARSE:
				//Orderly free everything we allocated during parsing.
				for(int k = 1; k <= accum.linecount; k++)
					free(accum.lines[k - 1]);

				free(accum.lines);
			case FAIL_RET_ALLOC:
				//Free the buffer.
				free(curr_line.contents);
			case FAIL_BUFF_ALLOC:
				//Construct the errored structure.
				accum.lines = NULL;
				accum.linecount = -1;
				break;
		}
	}


	return accum;
}

/*
mid_line *string_tokenizer(char **work) {

}

symbol *symbol_decoder(mid_line work) {

}

instruction *instruction_decoder(mid_line work) {

}

directive *directive_decoder(mid_line work) {

}

line *parse(FILE *work){
	
}
*/


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
