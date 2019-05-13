#include "data.h"

int isTokenDelim(char value);

uint32_t Hash (const char * data, int len);

family family_finder(char *work);

reg register_finder(char *in);

as_directive directive_finder(char *work);

char *strip_front(char *input, int size);

char *strip_back(char *input, int size);

char *copy_section(char *input, int start, int end);