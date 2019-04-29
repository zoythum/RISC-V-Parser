#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//include parse library function headers and data structures
#include "parse.h"

#define INITIAL_RETURNED_LINES_COLLECTION_SIZE 1000
#define RETURNED_LINES_COLLECTION_INCREMENT 500
#define INITIAL_INPUT_LINEBUFFER_SIZE 40

#define OBUFF_APPEND(C) obuff.oline[obuff.cursor++]=(C)

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
 * KEEP IN MIND that assembler preprocessing directives starting with a '#' are NOT supported and this function will error when one is found.
 * Be sure to clean your assembler source from such directives before parsing.
 *
 * EXTENSIONS USED:
 * - GNU: getline(), isblank()
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

	//Returned structure current maximum capacity
	int accum_cap;

	//Buffer for getline()'s read input
	struct linebuffer {
		char *contents;
		size_t buffer_size;
	} curr_line;

	//Input-reading functions return value
	int retval;

	/*Enumeration which specifies failures (or the lack thereof) during various phases of this function's execution:
	 * FAIL_RET_ALLOC: return structure initial allocation failed
	 * FAIL_BUFF_ALLOC: buffer initial allocation failed
	 * FAIL_PARSE: unrecoverable error encountered during parsing
	 * SUCCESS: no errors
	 */
	enum parsing_state { FAIL_RET_ALLOC, FAIL_BUFF_ALLOC, FAIL_PARSE, SUCCESS } status;


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
	accum_cap = INITIAL_RETURNED_LINES_COLLECTION_SIZE;


	/* EXEC */

	//Read first line to feel the ground
	errno = 0;
	retval = getline(&(curr_line.contents), &(curr_line.buffer_size), work);

	//Commence line processing
	while(retval != -1) {
		/* Syntax checking and input normalization is performed via an acceptor automata.
		 * START: we are about to parse a new construct
		 * SKIP: skip the current line
		 * COMMENT: what follows could be a line or multi-line comment
		 * MLC: we're traversing a multi-line comment
		 * MLES: we may be at the end of a multi-line comment
		 * DL: could be the start of either a directive or a label
		 * IL: could be the start of either an instruction or a label
		 * QL: currently verifying a quoted label
		 * QLE: at the end of a candidate quoted label
		 * DIRECTIVE: verifying the arguments of a directive
		 * INSTRUCTION: verifying the arguments of an assembler instruction
		 * REJECT: the input file is non-conforming
		 */
		enum { START, SKIP, COMMENT, MLC, MLES, DL, IL, QL, QLE, DIRECTIVE, INSTRUCTION, REJECT } acceptor_state = START;

		//Output buffer
		struct {
			char* oline;
			int cursor;
		} obuff;

		//Allocate the output buffer's actual buffer space with a size sufficient to hold all the characters on this line.
		obuff.oline = (char*) malloc((retval + 1) * sizeof(char));

		//Perform the usual allocation checks.
		if(obuff.oline == NULL && retval != 0) {
			status = FAIL_PARSE;
			errno = ENOMEM;
			goto CLNP;
		}

		obuff.cursor = 0;

		//Use the value returned by getline() to cycle over the characters.
		for(int c = 0; c < retval && acceptor_state != SKIP; c++) {
			char curr_char = curr_line.contents[c];

			switch(acceptor_state) {
				case START:
					//A symbol start was detected: check if it is valid and start making hypothesis.
					switch(curr_char) {
						case '\n':
							//Line terminated; wrap up buffering.
							curr_char = '\0';
							OBUFF_APPEND(curr_char);
							break;
						case ' ':
						case '\f':
						case '\r':
						case '\t':
						case '\v':
							//Whitespace compressor
							curr_char = ' ';
							OBUFF_APPEND(curr_char);

							do {
								c++;
								curr_char = curr_line.contents[c];
							} while(isblank(curr_char));

							//If we hit a non-whitespace character, unread it and continue.
							c--;
							break;
						case '.':
							//Could be a label or a directive
							acceptor_state = DL;
							OBUFF_APPEND(curr_char);
							break;
						case '"':
							//Should be a quoted label
							acceptor_state = QL;
							OBUFF_APPEND(curr_char);
							break;
						case '/':
							//What follows is a comment of some sort
							acceptor_state = COMMENT;
							break;
						default:
							if(isalpha(curr_char) || curr_char == '_' || curr_char == '$') {
								//Could be a label or an instruction
								acceptor_state = IL;
								OBUFF_APPEND(curr_char);
							}
							else {
								//Illegal character found
								acceptor_state = REJECT;
								c--;
							}
							break;
					}
					break;
				case QL:
					//Check if we're reading a possibly valid quoted label.
					if(curr_char == '\0') {
						//A quoted label can't contain a null character.
						acceptor_state = REJECT;
						c--;
					}
					else if(curr_char == '\"' && curr_line.contents[c - 1] != '\\'){
						//An un-escaped '"' means the end of the quoted symbol. Check if it isn't empty.
						if(c - 2 < 0 || (curr_line.contents[c - 2] != '\\' && curr_line.contents[c - 1] == '"')) {
							//Empty labels cannot be accepted
							acceptor_state = REJECT;
							c--;
						}
						else {
							//We might have reached the end of a valid quoted label.
							acceptor_state = QLE;
						}

						OBUFF_APPEND(curr_char);
					}
					break;
				case QLE:
					//Simply check if what has been read is really a label.
					if(curr_char == ':') {
						OBUFF_APPEND(curr_char);
						acceptor_state = START;
					}
					else {
						acceptor_state = REJECT;
						c--;
					}
					break;
				case DL:
				case IL:
					switch(curr_char) {
						case '\n':
							//Line is finished; wrap up.
							curr_char = '\0';
							OBUFF_APPEND(curr_char);
							acceptor_state = START;
							break;
						case ':':
							//It's a label
							OBUFF_APPEND(curr_char);
							acceptor_state = START;
							break;
						default:
							if(isalnum(curr_char) || curr_char == '.' || curr_char == '_' || curr_char == '$') {
								//Still inside symbol
								OBUFF_APPEND(curr_char);
							}
							else if(isblank(curr_char)) {
								//It's not a label
								curr_char = ' ';
								OBUFF_APPEND(curr_char);

								//Transit in the appropriate state
								acceptor_state = (acceptor_state == DL) ? DIRECTIVE : INSTRUCTION;
							}
							else {
								//Extraneous character detected; reject.
								acceptor_state = REJECT;
								c--;
							}
							break;
					}
					break;	
				case DIRECTIVE:
				case INSTRUCTION:
				case COMMENT:
					//Establish what kind of comment this is
					switch(curr_char) {
						case '/':
							//Line comment: skip the rest of the line.
							acceptor_state = SKIP;
							break;
						case '*':
							//Multi-line comment: transit to the appropriate state.
							acceptor_state = MLC;
							break;
						default:
							//Illegal character found
							acceptor_state = REJECT;
							c--;
							break;
					}
					break;
				case MLC:
					//Ignore everything but the '*' character
					if(curr_char == '*') {
						//Next character could be a comment terminator
						acceptor_state = MLES;
					}
					break;
				case MLES:
					if(curr_char == '/') {
						//Comment terminator reached: resume normal operation.
						acceptor_state = START;
					}
					else {
						//Merely a stray star: keep looking for a comment terminator.
						acceptor_state = MLC;
					}
					break;
				case REJECT:
					//The input file has been rejected: clean the output buffer and signal a parse failure.
					free(obuff.oline);
					status = FAIL_PARSE;
					goto CLNP;
			}
		}

		//If the buffer has useful content (neither an empty line nor a single-space line), copy its content to the output structure.
		if(!(obuff.cursor <= 2 && obuff.oline[0] == ' ')) {
			//Extend the returned lines array if necessary.
			if(accum.linecount == accum_cap) {
				char **holder = (char**) realloc(accum.lines, (accum_cap + RETURNED_LINES_COLLECTION_INCREMENT) * sizeof(char*));

				//Perform the correct allocation check, responding with an ordered deallocation to an eventual error.
				if(holder == NULL) {
					status = FAIL_PARSE;
					errno = ENOMEM;
					goto CLNP;
				}
				else {
					accum.lines = holder;
					accum_cap += RETURNED_LINES_COLLECTION_INCREMENT;
				}
			}

			//Allocate a new string in the returned collection.
			accum.lines[accum.linecount] = (char*) malloc(obuff.cursor * sizeof(char));

			//Perform the correct allocation check, responding with an ordered deallocation to an eventual error.
			if(accum.lines[accum.linecount] == NULL) {
				status = FAIL_PARSE;
				errno = ENOMEM;
				goto CLNP;
			}
			else {
				//Copy the output buffer contents into the newly allocated string.
				strncpy(accum.lines[accum.linecount], obuff.oline, obuff.cursor);
				accum.linecount++;
			}
		}

		//Free the output buffer
		free(obuff.oline);

		//If line was skipped, reset the state to START
		acceptor_state = (acceptor_state == SKIP) ? START : acceptor_state;

		//Continue with the next line (if any)
		errno = 0;
		retval = getline(&(curr_line.contents), &(curr_line.buffer_size), work);
	}

	//Check if the EOF was due to an error
	if((errno == ENOMEM) || (errno == EINVAL) || (ferror(work))) {
		//Proceed following the IO error handling pathway, propagating the error if possible.
		status = FAIL_PARSE;

		if((errno != ENOMEM) && (errno != EINVAL))
			errno = EIO;

		goto CLNP;
	}
	else {
		//We really are finished. Good job!
		status = SUCCESS;
	}


	/* CLEANUP */
CLNP:
	{
		char **swinger = accum.lines;

		switch(status) {
			case SUCCESS:
				//Compact the returned structure and free the buffer.
				accum.lines = realloc(accum.lines, accum.linecount * sizeof(char*));

				//If for some reason realloc() fails, proceed as if we errored during parsing.
				if ((accum.lines == NULL) && (accum.linecount != 0)){
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
