#include "lexer.h"
#include "automatas.c"

int lexer_flag = FALSE;

Token *tokens = NULL;
int num_tokens;
int comments;
int enum_index;

char *special_keys[] = {"+",  "-",  "*",  "/", "<", "<=", ">",
                        ">=", "==", "!=", "=", ";", ",",  "(",
                        ")",  "[",  "]",  "{", "}", "/*", "*/"};

char *keywords[] = {"else", "if", "int", "return", "void", "while"};

Token *lexer(FILE *file) {
  Buffer *buffer = allocate_buffer();
  initialize_lexer();
  char curr_char;

  while ((curr_char = getc(file))) {
    fill_buffer(buffer, curr_char);

    if (curr_char == EOF)
      break;
  }

  if (!num_tokens)
    print_lexic_error("Não há tokens válidos.");
  
  if (lexer_flag)
    print_tokens();
    
  deallocate_buffer(buffer);
  fclose(file);
  return tokens;
}

void initialize_lexer() {
  tokens = (Token *)malloc(sizeof(Token));

  num_tokens = 0;
  comments = 0;
  enum_index = 0;
}

void fill_buffer(Buffer *buffer, char ch) {
  buffer->data[buffer->char_pos] = ch;
  buffer->size++;
  buffer->char_pos++;

  // fim do buffer
  if (buffer->size == BUFFER_SIZE || ch == EOF || ch == '\n') {
    load_data(buffer);
    free(buffer->data);
    buffer->data = NULL;
    buffer->data = (char *)malloc(BUFFER_SIZE * sizeof(char));
    buffer->size = 0;
    buffer->char_pos = 0;
  }

  if (ch == '\n') {
    buffer->curr_line++;
  }
}

void load_data(Buffer *buffer) {
  char *curr_lexeme = (char *)malloc(buffer->size * sizeof(char));
  curr_lexeme[0] = '\0';
  int col = 1;
  buffer->char_pos = 0; // parte do inicio do array

  while (buffer->char_pos < buffer->size) {
    char *curr_char = get_next_char(buffer, 1);

    // delimitadores
    if (!strcmp(curr_char, " ") || !strcmp(curr_char, "\t") ||
        !strcmp(curr_char, "\n") || curr_char[0] == EOF) {
      process_lexeme(curr_lexeme, buffer->curr_line, col);

      curr_lexeme = reset_lexeme(buffer, curr_lexeme);
      col = buffer->char_pos + 1;
    } else if (is_special_key_or_keyword(curr_char, NUM_SPECIAL_KEYS, special_keys)) {
      process_lexeme(curr_lexeme, buffer->curr_line, col);
      
      char *temp = (char *)malloc(3 * sizeof(char));
      strcpy(temp, curr_char);
      
      char *next_char = get_next_char(buffer, 0);
      strcat(temp, next_char); // simbolo composto

      if (is_special_key_or_keyword(temp, NUM_SPECIAL_KEYS, special_keys)) {
        char *next_char = get_next_char(buffer, 1);
        strcat(curr_char, next_char);

        if (!strcmp(curr_char, "/*")) {
          comments++;
          continue;
        } else if (!strcmp(curr_char, "*/")) {
          comments--;
          continue;
        }
      }
      
      free(temp);
      free(next_char);

      process_lexeme(curr_char, buffer->curr_line, col);

      curr_lexeme = reset_lexeme(buffer, curr_lexeme);
      col = buffer->char_pos + 1;
    } else {
      strcat(curr_lexeme, curr_char);
    }
  }
}

char *reset_lexeme(Buffer *buffer, char *lexeme) {
  free(lexeme);
  char *new_lexeme = (char *)malloc(buffer->size * sizeof(char));
  new_lexeme[0] = '\0';

  return new_lexeme;
}

char *get_next_char(Buffer *buffer, int char_used) {
  char curr_char = buffer->data[buffer->char_pos];

  if (char_used && buffer->char_pos < buffer->size)
    buffer->char_pos++;

  // string com o caracter atual
  char *ch = (char *)malloc(2 * sizeof(char));
  ch[0] = curr_char;
  ch[1] = '\0';

  return ch;
}

void process_lexeme(char *lexeme, int line, int col) {
  // nao processa lexema de tamanho 0 ou contido em comentario
  if (!strlen(lexeme) || comments)
    return;

  Token token = get_token(lexeme, line, col);
  add_token(token);
}

Token get_token(char *lexeme, int line, int col) {
  Token token;

  if (is_special_key_or_keyword(lexeme, NUM_KEYWORDS, keywords)) {  // palavra reservada
    token = create_token(keywords[enum_index], lexeme, line, col);
  } else if (is_special_key_or_keyword(lexeme, NUM_SPECIAL_KEYS, special_keys)) {  // simbolo especial
    token = create_token(special_keys[enum_index], lexeme, line, col);
  } else if (run_automata(lexeme, t_id, accept_id, 2)) { // identifica identificador
    token = create_token("id", lexeme, line, col);
  } else if (run_automata(lexeme, t_num, accept_num, 2)) { // identifica numero
    token = create_token("num", lexeme, line, col);
  } else {
    char error_message[100];
    sprintf(error_message, "\"%s\" INVALIDO [linha: %d], COLUNA %d", lexeme,
           line, col);
    print_lexic_error(error_message);
  }

  return token;
}

int is_special_key_or_keyword(char *lexeme, int arr_size, char *arr[]) {
  for (int i = 0; i < arr_size; i++) {
    if (!strcmp(lexeme, arr[i])) {
      enum_index = i;
      return 1;
    }
  }

  return 0;
}

void print_tokens() {
  printf("# TOKENS\n\n");
  for (int i = 0; i < num_tokens; i++) {
    Token token = tokens[i];
    printf("Token: %s, Lexema: \"%s\" [linha: %d]\n", token.type, token.value,
           token.line);
  }
}

void print_lexic_error(char *message) {
  printf("ERRO LÉXICO: %s\n", message);
  exit(1);
}

Buffer *allocate_buffer() {
  Buffer *buffer = (Buffer *)malloc(sizeof(Buffer));
  buffer->data = (char *)malloc(BUFFER_SIZE * sizeof(char));
  buffer->size = BUFFER_SIZE;
  buffer->curr_line = 1;
  buffer->char_pos = 0;

  return buffer;
}

void deallocate_buffer(Buffer *buffer) {
  free(buffer->data);
  free(buffer);
}

Token create_token(char *type, char *value, int line, int col) {
  Token token;
  token.type = type;
  token.value = value;
  token.line = line;
  token.col = col;

  return token;
}

void add_token(Token token) {
  tokens = realloc(tokens, (num_tokens + 1) * sizeof(Token));
  tokens[num_tokens].type = strdup(token.type);
  tokens[num_tokens].value = strdup(token.value);
  tokens[num_tokens].line = token.line;
  tokens[num_tokens].col = token.col;
  num_tokens++;
}