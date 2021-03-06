//Include parse library function headers and data structures
#include "riscv-parse.h"

typedef struct {
	char **tokens;
	int token_num;
	roles role;
}mid_line;

//Structure containing the input file's meaningful normalized lines and their count.
typedef struct {
	char **lines;
	int linecount;
}input_lines;

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

#define INITIAL_RETURNED_LINES_COLLECTION_SIZE 1000
#define RETURNED_LINES_COLLECTION_INCREMENT 500
#define INITIAL_INPUT_LINEBUFFER_SIZE 40

#define OBUFF_APPEND(C) obuff.oline[obuff.cursor++]=(C)
#define COMPRESS compress_whitespaces=true
#define NOCOMPRESS compress_whitespaces=false

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

	{
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

		//Flag for enabling whitespace compression
		bool compress_whitespaces = true;

		//Commence line processing
		while(retval != -1) {

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

				//Whitespace compressor
				if(compress_whitespaces == true && isblank(curr_char)) {
					OBUFF_APPEND(' ');

					do {
						c++;
						curr_char = curr_line.contents[c];
					} while(isblank(curr_char));
				}

				switch(acceptor_state) {
					case START:
						//A symbol start was detected: check if it is valid and start making hypothesis.
						switch(curr_char) {
							case '\n':
								//Line terminated; wrap up buffering.
								OBUFF_APPEND('\0');
								break;
							case '.':
								//Could be a label or a directive
								acceptor_state = DL;
								OBUFF_APPEND(curr_char);

								//Don't miss separators
								NOCOMPRESS;
								break;
							case '"':
								//Should be a quoted label
								acceptor_state = QL;
								OBUFF_APPEND(curr_char);

								//Blank spaces are semantically relevant inside quotes
								NOCOMPRESS;
								break;
							case '/':
								//What follows is a comment of some sort
								acceptor_state = COMMENT;

								//A whitespace after a forward-slash is an invalid character
								NOCOMPRESS;
								break;
							default:
								if(isalpha(curr_char) || curr_char == '_' || curr_char == '$') {
									//Could be a label or an instruction
									acceptor_state = IL;
									OBUFF_APPEND(curr_char);

									//Watch out for separators
									NOCOMPRESS;
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
							COMPRESS;
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
								COMPRESS;
								break;
							case ':':
								//It's a label
								OBUFF_APPEND(curr_char);
								acceptor_state = START;
								COMPRESS;
								break;
							default:
								if(isalnum(curr_char) || curr_char == '.' || curr_char == '_' || curr_char == '$') {
									//Still inside symbol
									OBUFF_APPEND(curr_char);
								}
								else if(isspace(curr_char)) {
									//It's not a label
									curr_char = isspace(curr_char) ? ' ' : '\0';
									OBUFF_APPEND(curr_char);

									//Transit in the appropriate state
									acceptor_state = (acceptor_state == DL) ? DIRECTIVE : INSTRUCTION;

									//Re-enable whitespaces compressor
									COMPRESS;
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
						//Just copy whatever is in the statement until the line terminator is reached.
						//TODO remember that string constants can span multiple lines from quotes to quotes
						while(curr_char != '\n') {
							OBUFF_APPEND(curr_char);
							c++;
							curr_char = curr_line.contents[c];
						}

						OBUFF_APPEND('\0');
						acceptor_state = START;
						break;
					case INSTRUCTION:
						//Just normalize some known hiccups in the arguments.
						switch(curr_char) {
							case '\n':
								//The end
								OBUFF_APPEND('\0');
								acceptor_state = START;
								break;
							case '(':
								//In case of open parenthesis with no prefix value, add zero in front of it.
								if(obuff.oline[obuff.cursor - 1] == ',')
									OBUFF_APPEND('0');
							default:
								//Just... just keep copying, ok?
								OBUFF_APPEND(curr_char);
								break;
						}
						break;
					case COMMENT:
						//Establish what kind of comment this is
						switch(curr_char) {
							case '/':
								//Line comment: skip the rest of the line.
								acceptor_state = SKIP;
								COMPRESS;
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
							COMPRESS;
						}
						else {
							//Merely a stray star: keep looking for a comment terminator.
							acceptor_state = MLC;
						}
						break;
					default:
						//The input file has been rejected: clean the output buffer and signal a parse failure.
						free(obuff.oline);
						status = FAIL_PARSE;
						goto CLNP;
				}
			}

			//If the buffer has useful content (neither an empty line nor a single-space line), copy its content to the output structure.
			if(!(obuff.oline[0] == '\0' || (obuff.cursor <= 2 && obuff.oline[0] == ' '))) {
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

#undef INITIAL_RETURNED_LINES_COLLECTION_SIZE
#undef RETURNED_LINES_COLLECTION_INCREMENT
#undef INITIAL_INPUT_LINEBUFFER_SIZE

#undef OBUFF_APPEND
#undef COMPRESS
#undef NOCOMPRESS

/*
*	String tokenizer function, arguments are:
*	1) work, pointer to an input_line object containing the input file
*	2) output, pointer to a mid_line object's array, can be NULL
*	3) fill, integer defining how much of the output array has been filled
*	4) read, integer defining how many strings of the input have been read
*	5) output_size, specifies current size of output array
*	6) token, a string where a line is saved when the case "label + something" is encountered, otherwise is NULL
*/

#define TOKEN_A_MINUSC 'a'
#define TOKEN_Z_MINUSC 'z'

mid_line *string_tokenizer(input_lines work, mid_line *output, int fill, int read, int output_size, char *token, int *ssize) {
	mid_line *return_value = output;
	int size;
	int token_size;
	int return_token_size = 5;
	int token_count;
	int return_count;
	char *curr_tok;
	char *work_str;
	char *remaining_tok = NULL;

	//if return value is NULL it's allocated
	if (return_value == NULL) {
		size = work.linecount;
		*ssize = work.linecount;
		return_value = malloc(size*sizeof(mid_line));
		return_count = 0;
	} else {
		*ssize = output_size;
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
			token_size = strlen(work.lines[i]);
			work_str = malloc((token_size+1)*sizeof(char));
			strcpy(work_str, work.lines[i]);
			curr_tok = strtok(work_str, " ");
			token_size = strlen(curr_tok);
			return_value[return_count].tokens = malloc(return_token_size*sizeof(char*));
		} else {		
			token_size = strlen(token);
			work_str = malloc((token_size+1)*sizeof(char));
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
				token_size = strlen(curr_tok);
				remaining_tok = malloc((token_size+1)*sizeof(char));
				strcpy(remaining_tok, curr_tok);
				curr_tok = strtok(NULL, " ");
				while (curr_tok != NULL) {
					token_size = token_size + strlen(curr_tok);
					remaining_tok = realloc(remaining_tok, (token_size+2)*sizeof(char));
					remaining_tok = strcat(remaining_tok, " ");
					remaining_tok = strcat(remaining_tok, curr_tok);
					curr_tok = strtok(NULL, " ");	
				}
				return_value = realloc(return_value, (size+1)*sizeof(mid_line));
				return_value = string_tokenizer(work, return_value, return_count+1, i, size, remaining_tok, ssize);
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
		} else if ((curr_tok[0] >= TOKEN_A_MINUSC) && (curr_tok[token_size-1] <= TOKEN_Z_MINUSC)) {
			return_value[return_count].role = INSTRUCTION;
			return_value[return_count].tokens[token_count] = malloc((token_size + 1)*sizeof(char));
			strcpy(return_value[return_count].tokens[token_count], curr_tok);
			token_count++;
			curr_tok = strtok(NULL, " ");
			if (curr_tok != NULL) {
				token_size = strlen(curr_tok);
				return_value[return_count].tokens[token_count] = malloc((token_size+1)*sizeof(char));
				strcpy(return_value[return_count].tokens[token_count], curr_tok);
				token_count++;
			}
			return_value[return_count].token_num = token_count;
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

#undef TOKEN_A_MINUSC
#undef TOKEN_Z_MINUSC

/**
 * This function receives as input a mid_line which role is "symbol"
 * give as output a pointer to a label
 */
char *symbol_decoder(mid_line work) {
	char *label = malloc(50 * sizeof(char));
	int i = 0;
	char *setbool;
	/**
	 * We identify the name of the symbol searching for name's terminator (e.g. '', \0, :, =)
	 * all the character before the terminator are added to the name field of the output
	 */
	while ((work.tokens[0][i] != ' ') && (work.tokens[0][i] != '\0') && (work.tokens[0][i] != ':') && (work.tokens[0][i] != '=')){
		label[i] = work.tokens[0][i];
		i++;
	}

	return label;
}

/**
 * Each time a symbol is encountered the process of analizing it it's always the same
 * this function analize a symbol and updates its instruction input
 */
void symbol_analizer(instruction **input, char *symbol) {
	int symb_size;

	if (strlen(symbol) > 1) {
		if ((isdigit(symbol[0]) && isdigit(symbol[1])) || symbol[0] == '-') {
			(*input)->imm_field.literal = strtol(symbol, NULL, 10);
			(*input)->is_literal = true;
		} else if (isdigit(symbol[0]) && symbol[1] == 'x') {
			symbol = strip_front(symbol, 2);
			(*input)->imm_field.literal = strtol(symbol, NULL, 16);
			(*input)->is_literal = true;
		} else {
			symb_size = strlen(symbol) + 1;
			(*input)->imm_field.symb = malloc(symb_size*sizeof(char));
			strcpy((*input)->imm_field.symb, symbol);
			(*input)->is_literal = false;
		}
	} else {
		if (isdigit(symbol[0])) {
			(*input)->imm_field.literal = strtol(symbol, NULL, 10);
			(*input)->is_literal = true;
		} else {
			symb_size = strlen(symbol) + 1;
			(*input)->imm_field.symb = malloc(symb_size*sizeof(char));
			strcpy((*input)->imm_field.symb, symbol);
			(*input)->is_literal = false;
		}
	}
}

/**
 * instruction_decoder function, argument is:
 * 1) work, single mid_line object containing an instruction that has to be managed
 */
instruction *instruction_decoder(mid_line work) {
	char *opcode;
	char *symbol;
	char reg1[5], reg2[5], reg3[5];
	instruction *return_value = malloc(sizeof(instruction));
	int op_size = strlen(work.tokens[0]) + 1;
	int symb_size;
	int size, strip_size;
	int initial, final;
	char *ptr;
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
		case err: 
		/**
		 * family type: err
		 * This case handles an incorrect input
		 */
			printf("input is not correct: %s\n", work.tokens[0]);
			return NULL;
		case u: 
		/**
		 * family type: u
		 * In this case we have an argument string formatted as "register,offset"
		 */
			return_value->type = u;
			sscanf(work.tokens[1], "%[^,]", reg1);
			symbol = strip_front(work.tokens[1], strlen(reg1) + 1);
			symbol_analizer(&return_value, symbol);
			return_value->immediate = true;
			return_value->r1 = register_finder(reg1);
			return_value->r2 = unused;
			return_value->r3 = unused;
			break;
	    case li:
         /**
         * family type: li
         * A load immediate pseudo-instruction
         */
            return_value->type = li;
            sscanf(work.tokens[1], "%[^,]", reg1);
            symbol = strip_front(work.tokens[1], strlen(reg1) + 1);
            symbol_analizer(&return_value, symbol);
            return_value->immediate = true;
            return_value->r1 = register_finder(reg1);
            return_value->r2 = unused;
            return_value->r3 = unused;
            break;
		case i: 
		/**
		 * family type: i
		 * In this case we have an argument string formatted as "register,register,immediate"
		 */
			return_value->type = i;
			sscanf(work.tokens[1], "%[^,],%[^,]", reg1, reg2);
			symbol = strip_front(work.tokens[1], strlen(reg1) + strlen(reg2) + 2);
			symbol_analizer(&return_value, symbol);
			return_value->immediate = true;
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->r3 = unused;
			break;
		case s:
		/**
		 * family type: s
		 * In this case we have an argument string formatted as "register,offset(register)"
		 */
			return_value->immediate = true;
			return_value->type = s;
			initial = strcspn(work.tokens[1], ",");
			memcpy(reg1, work.tokens[1], initial);
			reg1[initial] = '\0';
			final = last_occurence(work.tokens[1], '(');
			if (final == -1) {
				printf("Missing (, must be an error\n");
				break;
			}
			ptr = copy_section(work.tokens[1], final+1, strlen(work.tokens[1])-1);
			strcpy(reg2, ptr);
			symbol = copy_section(work.tokens[1], strlen(reg1)+1, strlen(work.tokens[1])-strlen(reg2)-2);
			symbol_analizer(&return_value, symbol);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->r3 = unused;
			break;
		case r: 
		/**
		 * family type: r
		 * In this case we have an argument string formatted as "register,register,register"
		 */
			return_value->is_literal = false;
			return_value->immediate = false;
			return_value->type = r;
			sscanf(work.tokens[1], "%[^,],%[^,],%[^,]", reg1, reg2, reg3);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->r3 = register_finder(reg3);
			break;
		case j: 
		/**
		 * family type: j
		 * In this case we have an argument string formatted as "symbol"
		 * Because of the presence of a symbol we need to set the return_value symbol pointer to 
		 * the correct symbol in our global symbol table, first we check if it's present, then we set the pointer, otherwise 
		 * a new entry is created and added to the table
		 * Note, this family contains j and jal opcodes
		 */
			return_value->type = j;
			symbol = malloc((strlen(work.tokens[1])+1)*sizeof(char));
			sscanf(work.tokens[1], "%s", symbol);
			symbol[strlen(work.tokens[1])] = '\0';
			symbol_analizer(&return_value, symbol);
			return_value->immediate = true;
			return_value->r1 = unused;
			return_value->r2 = unused;
			return_value->r3 = unused;
			break;
		case jr: 
		/**
		 * family type jr
		 * In this case we have an argument string formatted as "register"
		 */
		// TODO what's the difference between jr instruction and s instruction? 
			return_value->is_literal = false;
			return_value->immediate = false;
			return_value->type = jr;
			sscanf(work.tokens[1],"%s", reg1);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = unused;
			return_value->r3 = unused;
			break;
		case b: 
		/**
		 * family type: b
		 * In this case we have an argument string formatted as "register,register,symbol"
		 */
			return_value->type = b;
			sscanf(work.tokens[1],"%[^,],%[^,]", reg1, reg2);
			symbol = strip_front(work.tokens[1], strlen(reg1) + strlen(reg2) + 2);
			symbol_analizer(&return_value, symbol);
			return_value->immediate = true;
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->r3 = unused;
			break;
		case al:
		/**
		 * family type: al
		 * In this case we have an argument string formatted as "register,offset(register)"
		 */
		// TODO what's the difference between al instruction and s instruction? 
			return_value->type = al;
			initial = strcspn(work.tokens[1], ",");
			memcpy(reg1, work.tokens[1], initial);
			reg1[initial] = '\0';
			final = last_occurence(work.tokens[1], '(');
			ptr = copy_section(work.tokens[1], final+1, strlen(work.tokens[1])-1);
			strcpy(reg2, ptr);
			symbol = copy_section(work.tokens[1], strlen(reg1)+1, strlen(work.tokens[1])-strlen(reg2)-2);
			return_value->immediate = true;
			symbol_analizer(&return_value, symbol);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->r3 = unused;
			break;
		case as: 
		/**
		 * family type: as
		 * In this case we have an argument string formatted as "register,register,offset(register)"
		 */
			return_value->type = as;
			sscanf(work.tokens[1],"%[^,],%[^,],%*s", reg1, reg2);
			symbol = strip_front(work.tokens[1], strlen(reg1)+strlen(reg2)+2);
			final = last_occurence(symbol, '(');
			ptr = copy_section(symbol, final+1, strlen(symbol)-1);
			strcpy(reg3, ptr);
			symbol = strip_back(symbol, strlen(reg3) + 2);
			symbol_analizer(&return_value, symbol);
			return_value->immediate = true;
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->r3 = register_finder(reg3); 
			break;
		case sext: 
		/**
		 * family type: sext
		 * In this case we have an argument string formatted as "register,register"
		 * Note that this is a pseudo-instruction expanding to "addi reg1,reg2,0"
		 */
			return_value->is_literal = true;
			return_value->type = sext;
			sscanf(work.tokens[1],"%[^,],%[^,]", reg1, reg2);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->r3 = unused;
			return_value->imm_field.literal = 0;
			return_value->immediate = true;
			break;
		case _2arg:
		/**
		 * family type: _2arg
		 * In this case we have an argument string formatted as "register,unknown"
		 * First we must find out if the unknown value is a register or a symbol
		 */
			return_value->is_literal = false;
			return_value->type = _2arg;
			sscanf(work.tokens[1],"%[^,],", reg1);
			symbol = strip_front(work.tokens[1], strlen(reg1)+1);
			if (isdigit(symbol[0]) || symbol[0] == '-') {
				return_value->is_literal = true;
				return_value->imm_field.literal = strtol(symbol, NULL, 10);
				return_value->immediate = true;
				return_value->r1 = register_finder(reg1);
				return_value->r2 = unused;
				return_value->r3 = unused;
			} else {
				return_value->is_literal = false;
				return_value->immediate = false;
				strcpy(reg2, symbol);
				return_value->r1 = register_finder(reg1);
				return_value->r2 = register_finder(reg2);
				return_value->r3 = unused;
			}
			break;
		case bz:
		/**
		 * family type: bz
		 * In this case we have an argument string formatted as "register,symbol"
		 */
			return_value->type = bz;
			sscanf(work.tokens[1], "%[^,],", reg1);
			symbol = strip_front(work.tokens[1], strlen(reg1)+1);
			return_value->immediate = true;
			if (isdigit(symbol[0])) {
				return_value->is_literal = true;
				return_value->imm_field.literal = strtol(symbol, NULL, 10);
				return_value->r1 = register_finder(reg1);
				return_value->r2 = unused;
				return_value->r3 = unused;
			} else {
				return_value->is_literal = false;
				return_value->imm_field.symb = malloc((strlen(symbol)+1)*sizeof(char));
				strcpy(return_value->imm_field.symb, symbol);
				return_value->r1 = register_finder(reg1);;
				return_value->r2 = unused;
				return_value->r3 = unused;
			}   
			break;
		case nop:
			/**
			 * family type: nop
			 * We do not have any arguments here
			 */
			return_value->type = nop;
			return_value->is_literal = false;
			return_value->immediate = false;
			return_value->r1 = unused;
			return_value->r2 = unused;
			return_value->r3 = unused;
			break;
		case snez:
			/**
			 * family type: snez
			 * Arguments formatted as "register, register"
			 */
			return_value->is_literal = false;
			return_value->type = snez;
			sscanf(work.tokens[1],"%[^,],%[^,]", reg1, reg2);
			return_value->r1 = register_finder(reg1);
			return_value->r2 = register_finder(reg2);
			return_value->r3 = unused;
			return_value->immediate = false;
			break;
		default:
			break;
	}
	return (return_value);
}

/**
 * directive_decoder function, argument is:
 * 1) work, single mid_line object containing a directive that has to be managed
 */ 
directive *directive_decoder(mid_line work) {
	int count = 1;
	char *ptr;
	directive *output;
	output = malloc(sizeof(directive));
	output->args = malloc((work.token_num-1)*sizeof(char*));
	output->args_num = work.token_num - 1;
	ptr = strip_front(work.tokens[0], 1);
	output->name = directive_finder(ptr);
	while (count < work.token_num) {
		ptr = strip_back(work.tokens[count], 0);
		if (ptr[strlen(ptr)-1] == ',') {
			ptr = strip_back(work.tokens[count], 1);
		}
		output->args[count-1] = malloc((strlen(ptr)+1)*sizeof(char));
		strcpy(output->args[count-1], ptr);
		count++;
	}
	return(output);
}

/**
 * Check if specified symbol has already been memorized in lab_tab
 */
// TODO: test this function
bool find_symb(lab_tab *head, char *symb) {
	lab_tab *curr;
	curr = head;
	while (curr != NULL) {
		if ((curr->label != NULL) && (strcmp(curr->label, symb) == 0)) {
			return true;
		}

		curr = curr->next;
	}
	return (false);
}

/**
 * Parse function, takes a FILE pointer as input
 */
line_encaps *parse(FILE *work){
	/**
	 * ssize will contain the size of out mid_line array
	 */
	int *ssize;
	int i = 0;
	line *head, *curr;
	lab_tab *head_s, *curr_s;
	line_encaps *out;
	input_lines first;
	mid_line *second;
	second = NULL;
	out = malloc(sizeof(line_encaps));
	ssize = malloc(sizeof(int));
	head = malloc(sizeof(line));
	head->prev_line = NULL;
	head->next_line = NULL;
	head_s = malloc(sizeof(lab_tab));
	head_s->label = NULL;
	head_s->next = NULL;
	head_s->prev = NULL;
	curr_s = head_s;
	curr = head;
	/**
	 * first we clean our input file, then everything is tokenized
	 */
	first = line_feeder(work);
	second = string_tokenizer(first, second, 0, 0, first.linecount, NULL, ssize);

	/**
	 * For each line obtained we call the right function and a linked list is created
	 */
	for (i = 0; i < *ssize; i++) {
		/**
		 * if we are dealing with a symbol we make sure that, 
		 * if not already there, it's inserted into the symbol table
		 */
		if (second[i].role == LABEL) {
			curr->role = LABEL;
			curr->ptr.label = symbol_decoder(second[i]);
			curr->next_line = NULL;
			if (! find_symb(head_s, curr->ptr.label)) {
				curr_s->label = symbol_decoder(second[i]);
				curr_s->next = malloc(sizeof(lab_tab));
				curr_s->next->label = NULL;
				curr_s->next->prev = curr_s;
				curr_s = curr_s->next;
				curr_s->next = NULL;
			}
		} else if (second[i].role == DIRECTIVE) {
			curr->role = DIRECTIVE;
			curr->ptr.dir = directive_decoder(second[i]);
			curr->next_line = NULL;
		} else if (second[i].role == INSTRUCTION) {
			curr->role = INSTRUCTION;
			curr->ptr.instr = instruction_decoder(second[i]);
			curr->next_line = NULL;
		}
		curr->next_line = malloc(sizeof(line));
		curr->next_line->prev_line = curr;
		curr = curr->next_line;
	}
	//Horrible hotfix follows
	curr->prev_line->next_line = NULL;
	free(curr);
	//TODO remove that shit asap

	out->line_head = head;
	out->symbol_head = head_s;
	return (out);
}

/*
 * Generates an assembler source from a line_encaps-contained intermediate ASM representation, outputting it on the provided stream.
 * When succesful, this function outputs the number of lines written to the output stream.
 *
 * In case of failure, -1 is returned and no attempt is made to reset the stream, since such operation could be unsupported or meaningless.
 */

#define IMM_PRINT(S,I) if((I) -> is_literal) \
				fprintf((S), "%d", (I) -> imm_field.literal); \
		       else \
				fprintf((S), "%s", (I) -> imm_field.symb)

int rebuild(struct line_encaps material, FILE *output) {
	//Number of lines written to output
	int written_lines = 0;

	//Set scan start at the head of the line chain.
	line *curr_line = material.line_head;

	//Scan through the materials until they are exhausted.
	char *symb;
	directive *dir;
	instruction *inst;
	while(curr_line != NULL) {
		switch(curr_line -> role) {
			case LABEL:
				symb = curr_line -> ptr.label;

				//Start printing the symbol
				fprintf(output, "%s\n", symb);

				break;
			case DIRECTIVE:
				dir = curr_line -> ptr.dir;

				//Convert the enumerated identifier back to a string and print it.
				fprintf(output, "%s", dir_tostring(dir -> name));

				if(dir -> args_num > 0) {
					//Loop-print all of the arguments except the last one
					for(int narg = 0; narg < dir -> args_num - 1; narg++)
						fprintf(output, " %s,", dir -> args[narg]);

					//Print the last argument and end the current line
					fprintf(output, " %s\n", dir -> args[dir -> args_num - 1]);
				}
				else
					fprintf(output, "\n");

				break;
			case INSTRUCTION:
				inst = curr_line -> ptr.instr;

				//Whatever this instruction may be, its opcode doesn't need to be processed.
				fprintf(output, "%s", inst -> opcode);

				//Each instruction family has a somewhat different operands layout. Let's treat them differently.
				switch(inst -> type) {
					case u:
					case bz:
						//register,immediate
						fprintf(output, " %s,", reg_tostring(inst -> r1));
						IMM_PRINT(output, inst);
						fprintf(output, "\n");

						break;
					case i:
					case b:
						//register,register,immediate
						fprintf(output, " %s,%s,", reg_tostring(inst -> r1), reg_tostring(inst -> r2));
						IMM_PRINT(output, inst);
						fprintf(output, "\n");

						break;
					case s:
					case al:
						//register,offset(register)
						fprintf(output, " %s,", reg_tostring(inst -> r1));
						IMM_PRINT(output, inst);
						fprintf(output, "(%s)\n", reg_tostring(inst -> r2));

						break;
					case r:
						//register,register,register
						fprintf(output, " %s,%s,%s\n", reg_tostring(inst -> r1), reg_tostring(inst -> r2), reg_tostring(inst -> r3));

						break;
					case j:
						//immediate
						fprintf(output, " ");
						IMM_PRINT(output, inst);
						fprintf(output, "\n");

						break;
					case jr:
						//register
						fprintf(output, " %s\n", reg_tostring(inst -> r1));

						break;
					case as:
						//register,register,offset(register)
						fprintf(output, " %s,%s,", reg_tostring(inst -> r1), reg_tostring(inst -> r2));
						IMM_PRINT(output, inst);
						fprintf(output, "(%s)\n", reg_tostring(inst -> r3));

						break;
					case sext:
						//register,register
						fprintf(output, " %s,%s\n", reg_tostring(inst -> r1), reg_tostring(inst -> r2));

						break;
					case _2arg:
						//register,unknown
						//Something very strange...
						if(inst -> is_literal)
							fprintf(output, " %s,%d\n", reg_tostring(inst -> r1), inst -> imm_field.literal);
						else
							fprintf(output, " %s,%s\n", reg_tostring(inst -> r1), reg_tostring(inst -> r2));
						break;
					case nop:
						//*empty*
						fprintf(output, "\n");

						break;
					case snez:
						fprintf(output, " %s,%s\n", reg_tostring(inst -> r1), reg_tostring(inst -> r2));

						break;
					default:
						//Something is definitely broken...
						return -1;
				}

				break;
			default:
				//Unrecognized role: signal the error and exit
				return -1;
		}

		//Proceed to the next item, incrementing the line count.
		written_lines++;
		curr_line = curr_line -> next_line;
	}

	//Input exhausted: return number of written lines
	return written_lines;
}

#undef IMM_PRINT
