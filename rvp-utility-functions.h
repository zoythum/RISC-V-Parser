#ifndef PARSER_UTIL_FUNC_H
#define PARSER_UTIL_FUNC_H

#include "rvp-data.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t Hash (const char * data, int len);

family family_finder(char *work);

reg register_finder(char *in);

as_directive directive_finder(char *work);

char *strip_front(char *input, int size);

char *strip_back(char *input, int size);

char *copy_section(char *input, int start, int end);

int last_occurence(char *input, char value);

char *reg_tostring(reg enreg);

char *dir_tostring(as_directive endir);

char *fam_tostring(family fam);

#endif /* PARSER_UTIL_FUNC_H */
