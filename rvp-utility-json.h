#include <stdio.h>
#include "riscv-parse.h"

void label_to_json(symbol input, FILE *output);

void directive_to_json(directive input, FILE *output);

void instruction_to_json(instruction input, FILE *output);

void export_to_json(struct line_encaps input, FILE *output);