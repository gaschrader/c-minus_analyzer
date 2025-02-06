#ifndef COMMON_H
#define COMMON_H

#define TRUE 1
#define FALSE 0

extern int lexer_flag;
extern int parser_flag;
extern int semantic_flag;

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// estrutura dos tokens
typedef struct {
  char *type;
  char *value;
  int line;
  int col;
} Token;

// variaveis globais para todos os 
// analisadores terem acesso
extern Token *tokens;
extern int num_tokens;

extern Token curr_token;
extern int token_pos;

// estrutura dos nós da árvore sintática
typedef struct Node {
  char *value;
  int line;
  struct Node *child; 
  struct Node *sibling;
} Node;

#endif