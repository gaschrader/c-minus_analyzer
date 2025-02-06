#ifndef LEXER_H
#define LEXER_H

#include "common.h"

#define NUM_SPECIAL_KEYS 21
#define BUFFER_SIZE 256
#define NUM_KEYWORDS 6

// estruturas

// enum para palavras-chave

enum Keyword { ELSE, IF, INT, RETURN, VOID, WHILE, ID, NUM };

extern char *keywords[];

// enum para caracteres especiais

enum SpecialKey {
  PLUS,
  MINUS,
  TIMES,
  DIVIDE,
  LESS,
  LESS_OR_EQUAL,
  GREATER,
  GREATER_OR_EQUAL,
  EQUAL,
  DIFFERENT,
  ASSIGN,
  SEMICOLON,
  COMMA,
  OPEN_PARENTHESIS,
  CLOSE_PARENTHESIS,
  OPEN_BRACKETS,
  CLOSE_BRACKETS,
  OPEN_BRACES,
  CLOSE_BRACES,
  OPEN_COMMENT,
  CLOSE_COMMENT
};

extern char *special_keys[];

typedef struct {
  char *data;
  int size;
  int curr_line;
  int char_pos;
} Buffer;

// funções do analisador léxico

Token *lexer(FILE *file);
void initialize_lexer();
char *reset_lexeme(Buffer *buffer, char *lexeme);
char *get_next_char(Buffer *buffer, int char_used);
void process_lexeme(char *lexeme, int line, int col);
Token get_token(char *lexeme, int line, int col);
int is_special_key_or_keyword(char *lexeme, int arr_size, char *arr[]);
void print_tokens();
void print_lexic_error(char *message);

// funções de Buffer

Buffer *allocate_buffer();
void deallocate_buffer(Buffer *buffer);
void fill_buffer(Buffer *buffer, char curr_char);
void load_data(Buffer *buffer);

// funções de Token

Token create_token(char *type, char *value, int line, int col);
void add_token(Token token);

#endif