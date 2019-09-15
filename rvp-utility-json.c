#include "rvp-utility-json.h"

//Convert label into json object and writes it in specified output file
void label_to_json(char *input, FILE *output) {
    fprintf(output, "{\"role\" : \"label\", \"name\" : \"%s\"}\n", input);
}

//Convert directive into json object and writes it in specified output file
void directive_to_json(directive input, FILE *output) {
    //First we write all the standard data, then all the arguments are written as elements of an array.
    fprintf(output, "{\"role\" : \"directive\", \"name\" : \"%s\", \"args\" : [", dir_tostring(input.name));
    for (int i = 0; i < input.args_num; i++) {
	    putc('\"', output);

	    for(int c = 0; input.args[i][c] != '\0'; c++) {
            //If a double quote is encountered we make sure to correctly escape it so that the JSON file is correctly formatted
		    if(input.args[i][c] == '\"')
			    putc('\\', output);

		    putc(input.args[i][c], output);
	    }

	    putc('\"', output);

        if (i < input.args_num-1) {
            fprintf(output, ",");
        }
    }
    fprintf(output, "]}");
}

//Convert instruction into json object and writes it in specified output file
void instruction_to_json(instruction input, FILE *output) {
    //Depending on the value of "immediate" field different data will be written so that each family type is managed correctly
    if (input.immediate) {
        if (input.is_literal) {
            fprintf(output, "{\"role\" : \"instruction\", \"opcode\" : \"%s\", \"r1\" : \"%s\", \"r2\" : \"%s\", \"r3\" : \"%s\", \"immediate\" : %d, \"family\" : \"%s\"}",
                    input.opcode, reg_tostring(input.r1), reg_tostring(input.r2), reg_tostring(input.r3), input.imm_field.literal, fam_tostring(input.type));
        } else {
            fprintf(output, "{\"role\" : \"instruction\", \"opcode\" : \"%s\", \"r1\" : \"%s\", \"r2\" : \"%s\", \"r3\" : \"%s\", \"immediate\" : \"%s\", \"family\" : \"%s\"}",
                    input.opcode, reg_tostring(input.r1), reg_tostring(input.r2), reg_tostring(input.r3), input.imm_field.symb, fam_tostring(input.type));
        }
    } else {
        fprintf(output, "{\"role\" : \"instruction\", \"opcode\" : \"%s\", \"r1\" : \"%s\", \"r2\" : \"%s\", \"r3\" : \"%s\", \"immediate\" : \"None\", \"family\" : \"%s\"}",
                    input.opcode, reg_tostring(input.r1), reg_tostring(input.r2), reg_tostring(input.r3), fam_tostring(input.type));
    }
    
}

/**
 * This function exports a line_encaps struct to a JSON file.
 * The top structure will be an array of JSON objects
 */
void export_to_json(struct line_encaps input, FILE *output) {
    line *head;
    head = input.line_head;

    //first we print a square bracket to initialize the top structure
    fprintf(output, "[");

    //We check if the input contains valid data, if so we begin with the export operations dividing into three different functions depending on the line's role
    if(head != NULL) {
	    if (head->role == INSTRUCTION) {
		    instruction_to_json(*(head->ptr.instr), output);
	    } else if (head->role == DIRECTIVE) {
		    directive_to_json(*(head->ptr.dir), output);
	    } else if (head->role == LABEL) {
		    label_to_json(head->ptr.label, output);
	    }

	    head = head->next_line;
	    while (head != NULL) {
		    fprintf(output, ",");

		    if (head->role == INSTRUCTION) {
			    instruction_to_json(*(head->ptr.instr), output);
		    } else if (head->role == DIRECTIVE) {
			    directive_to_json(*(head->ptr.dir), output);
		    } else if (head->role == LABEL) {
			    label_to_json(head->ptr.label, output);
		    }

		    head = head->next_line;
	    }
    }

    //We make sure to add a closing square braket at the end
    fprintf(output, "]");
}
