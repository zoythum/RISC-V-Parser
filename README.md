# Parser progetto ing. informatica

----
## Proposed IR

The data structure we propose is composed of four different fields:

    typedef struct{
       int line_num;
       int token_count;
       char **tokens;
       roles role;
    }line;


* an integer value containing the line number
* an integer value containing the number of tokens on the line
* a string array (tokens)
* a field of `role` type identifying the line's meaning:
```
typedef enum {INSTRUCTION, DIRECTIVE, LABEL, COMMENT} role;
```

## Usage
The proposed parser takes as input a `FILE*` containing a reference to an assembler source, reads its content and returns a `line` array. 

