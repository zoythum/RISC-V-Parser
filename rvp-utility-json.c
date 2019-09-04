#include "rvp-utility-json.h"

//Convert label into json object and writes it in specified output file
void label_to_json(symbol input, FILE *output) {
    fprintf(output, "{\"role\" : \"label\", \"name\" : \"%s\", \"value\" : \"%d\"}\n", input.name, input.value);
}

//Convert directive into json object and writes it in specified output file
void directive_to_json(directive input, FILE *output) {
    fprintf(output, "{\"role\" : \"directive\", \"name\" : \"%s\", \"args\" : [", dir_tostring(input.name));
    for (int i = 0; i < input.args_num; i++) {
	    putc('\"', output);

	    for(int c = 0; input.args[i][c] != '\0'; c++) {
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
    if (input.immediate) {
        if (input.is_literal) {
            fprintf(output, "{\"role\" : \"instruction\", \"opcode\" : \"%s\", \"r1\" : \"%s\", \"r2\" : \"%s\", \"r3\" : \"%s\", \"immediate\" : \"%d\", \"family\" : \"%s\"}", 
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

void export_to_json(struct line_encaps input, FILE *output) {
    line *head;
    head = input.line_head;

    fprintf(output, "[");

    if(head != NULL) {
	    if (head->role == INSTRUCTION) {
		    instruction_to_json(*(head->ptr.instr), output);
	    } else if (head->role == DIRECTIVE) {
		    directive_to_json(*(head->ptr.dir), output);
	    } else if (head->role == LABEL) {
		    label_to_json(*(head->ptr.sym), output);
	    }

	    head = head->next_line;
	    while (head != NULL) {
		    fprintf(output, ",");

		    if (head->role == INSTRUCTION) {
			    instruction_to_json(*(head->ptr.instr), output);
		    } else if (head->role == DIRECTIVE) {
			    directive_to_json(*(head->ptr.dir), output);
		    } else if (head->role == LABEL) {
			    label_to_json(*(head->ptr.sym), output);
		    }

		    head = head->next_line;
	    }
    }

    fprintf(output, "]");
}
