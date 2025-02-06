#include "semantic.h"

int semantic_flag = FALSE;

ScopeStack *stack;
ScopeTable *global_scope;

void build_table(Node *root) {
  initialize_semantic();
  create_full_table(root);

  if (semantic_flag)
    display_table();

  free_scope_table(global_scope);
  free_stack(stack);
}

void semantic_verification(Node *root) {
  verify_tree(root);
  free_tree(root);
}

int has_return = FALSE;
char *var_type;
char *func_type;

void initialize_semantic() {
  stack = create_stack(0);
  global_scope = create_scope_table("global");
  HashTable *table = create_table(HASH_TABLE_SIZE);

  global_scope->table = table;
  
  insert(table, new_symbol("input", "-", "func", "int"));
  insert(table, new_symbol("output", "-", "func", "void"));
  
  push(stack, global_scope);
}

void create_full_table(Node *node) {
  ScopeTable *stack_head = stack->items[stack->top];
  HashTable *curr_table = stack_head->table;
  
  if (node == NULL) 
    return;

  // declaracao funcao
  if (!strcmp(node->value, "fun_decl")) {
    handle_declaration(1, node, stack_head, curr_table);
  }
  
  // declaracao variavel
  if (!strcmp(node->value, "var_decl") || !strcmp(node->value, "param")) {
    handle_declaration(2, node, stack_head, curr_table);
  }

  // ocorrencia de funcao ou variavel
  if (!strcmp(node->value, "var") || !strcmp(node->value, "ativacao")) {
    char *key = node->child->value;
    Symbol *sym = find_symbol(curr_table, key);
    
    if (sym) {
      add_line(sym->lines, node->child->line);
    
      if (!strcmp(node->value, "var"))
        var_type = sym->data_type;

      if (!strcmp(node->value, "ativacao"))
        func_type = sym->data_type;
    }
    else {
      check_parent_table(stack_head, key, node->child->line);
    }
  }

  // verifica se os tipos coincidem
  if (var_type && func_type) {
    if (strcmp(var_type, func_type)) {  // sao diferentes
      char error_message[200];
      sprintf(error_message, "retorno do tipo '%s' não pode ser atribuído a variável de tipo '%s'", func_type, var_type);
      print_semantic_error(error_message, node->line);
    }
    var_type = func_type = NULL;
  }

  set_scope_action(stack_head, node);

  create_full_table(node->child);
  create_full_table(node->sibling);
}

void handle_declaration(int decl_type, Node *node, ScopeTable *stack_head, HashTable *curr_table) {
  Symbol *sym = NULL;
  char error_message[200];
  char *name = node->child->sibling->value;
  int line = node->child->sibling->line;
  
  if (decl_type == 1) {  // func
    sym = new_symbol(node->child->sibling->value, "-", "func", node->child->child->value);
    sprintf(error_message, "função \'%s\' já declarada", name);
  } else if (decl_type == 2) {  // var
    sym = new_symbol(node->child->sibling->value, stack_head->name, "var", node->child->child->value);
    sprintf(error_message, "variável \'%s\' já declarada neste escopo", name);
  }

  if (find_symbol(curr_table, sym->name) == NULL)
    insert(curr_table, sym);
  else
    print_semantic_error(error_message, line);

  add_line(sym->lines, line);
}

void set_scope_action(ScopeTable *stack_head, Node *node) {
  if (!strcmp(node->value, "fun_decl")) {
    char *func_name = node->child->sibling->value;
    add_new_scope(stack_head, func_name);
  }
  else if (!strcmp(node->value, "if") ||
    !strcmp(node->value, "while") ||
    !strcmp(node->value, "else")) {
    add_new_scope(stack_head, node->value);
  }
  else if (!strcmp(node->value, "}")) {
    pop(stack);
  }
}

void check_parent_table(ScopeTable *stack_head, char *key, int line) {
  if (stack_head->parent) {
    Symbol *sym = find_symbol(stack_head->parent->table, key);

    if (sym)
      add_line(sym->lines, line);
    else
      check_parent_table(stack_head->parent, key, line);
  }
  else {
    char error_message[200];
    sprintf(error_message, "variável ou função \'%s\' ainda não declarada neste escopo", key);
    print_semantic_error(error_message, line);
  }
}

void add_new_scope(ScopeTable *stack_head, char *scope_name) {
  if (stack_head->child == NULL) {
    stack_head->child = create_scope_table(scope_name);
  }
  else {
    ScopeTable *curr_table = stack_head->child;

    while (curr_table->sibling)
      curr_table = curr_table->sibling;

    curr_table->sibling = create_scope_table(scope_name);
  }
}

void display_table() {
  printf("# TABELA DE SIMBOLOS\n\n");
  printf("Nome_ID;Escopo;Tipo_ID;Tipo_dado;Linhas\n\n");
  print_scope_table(global_scope);
}

// funcao recursiva que printa cada tabela de escopos
void print_scope_table(ScopeTable *table) {
  if (table == NULL)
    return;

  for (int i = 0; i < table->table->size; i++) {
    Symbol *symbol = table->table->items[i];

    if (symbol != NULL) {
      printf("%s;%s;%s;%s;", symbol->name, symbol->scope_name, symbol->id_type, symbol->data_type);
      print_lines(symbol->lines);
    }
  }

  print_scope_table(table->child);
  print_scope_table(table->sibling);
}

void print_lines(LineList *lines) {
  for(int i = 0; i < lines->size; i++) {
    if (i == lines->size - 1)
      break;
    
    printf("%d,", lines->list[i]);
  }

  printf("%d\n", lines->list[lines->size - 1]);
}

// funcao recursiva que verifica a arvore sintatica
void verify_tree(Node *node) {
  if (node == NULL)
    return;

  // verifica se o tipo da variável é valido
  verify_var_type(node);

  // verifica se a função deve ou não ter retorno
  verify_func_return(node);

  // verifica se o retorno é valido
  verify_return(node);

  verify_tree(node->child);
  verify_tree(node->sibling);
}

void verify_var_type(Node *node) {
  if (!strcmp(node->value, "var_decl")) {
    char *var_type = node->child->child->value;

    if (!strcmp(var_type, "void")) {
      char *var_name = node->child->sibling->value;
      int var_line = node->child->child->line;
      
      char error_message[100];
      sprintf(error_message, "ERRO SEMÂNTICO: variável %s não pode ser do tipo 'void'", var_name);
      print_semantic_error(error_message, var_line);
    }
  }
}

void verify_func_return(Node *node) {
  if (!strcmp(node->value, "fun_decl")) {
    char *fun_type = node->child->child->value;

    if (!strcmp(fun_type, "int"))
      has_return = TRUE;
    else if (!strcmp(node->value, "void"))
      has_return = FALSE;
  }
}

void verify_return(Node *node) {
  if (!strcmp(node->value, "return")) {
    if (has_return && !strcmp(node->sibling->value, ";"))
      print_semantic_error("essa função exige retorno", node->line);
    
    if (!has_return && !strcmp(node->sibling->value, "expressao"))
      print_semantic_error("essa função não possui retorno", node->line);
  }
}

void print_semantic_error(char *message, int line) {
  printf("ERRO SEMÂNTICO: %s. LINHA: %d\n", message, line);
  exit(1);
}

Symbol *new_symbol(char *name, char *scope_name, char *id_type, char *data_type) {
  Symbol *sym = (Symbol *)malloc(sizeof(Symbol));
  sym->name = name;
  sym->scope_name = scope_name;
  sym->id_type = id_type;
  sym->data_type = data_type;
  sym->lines = (LineList *)malloc(sizeof(LineList));
  sym->lines->list = (int *)malloc(sizeof(int));
  sym->lines->size = 0;

  return sym;
}

void add_line(LineList *lines, int value) {
  for (int i = 0; i < lines->size; i++) {
    if (lines->list[i] == value)
      return;  // linha ja adicionada
  }

  lines->list[lines->size] = value;
  lines->list = (int *)realloc(lines->list, sizeof(int) * (lines->size + 1));
  lines->size++;
}

HashTable *create_table(int size) {
  HashTable *table = (HashTable *)malloc(sizeof(HashTable));
  table->size = size;
  table->items = (Symbol **)malloc(sizeof(Symbol *) * size);

  return table;
}

void insert(HashTable *table, Symbol *symbol) {
  unsigned int index = hash(symbol->name, table->size);
  table->items[index] = symbol;
}

unsigned int hash(const char *key, int size) {
  unsigned int hash = 0;

  while (*key)
    hash = (hash * 31) + *key++;

  return hash % size;
}

Symbol *find_symbol(HashTable *table, char *name) {
  unsigned int index = hash(name, table->size);
  Symbol *symbol = table->items[index];

  if (symbol != NULL) {
    if(!strcmp(symbol->name, name)) 
      return symbol;
  }

  return NULL;
}

ScopeTable *create_scope_table(char *name) {
  ScopeTable *scope_table = (ScopeTable *)malloc(sizeof(ScopeTable));
  scope_table->name = name;
  scope_table->table = create_table(HASH_TABLE_SIZE);
  scope_table->parent = (stack->top == -1) ? NULL : stack->items[stack->top];
  scope_table->child = NULL;
  scope_table->sibling = NULL;

  push(stack, scope_table);

  return scope_table;
}

ScopeStack *create_stack(int size) {
  ScopeStack *stack = (ScopeStack *)malloc(sizeof(ScopeStack));
  stack->size = size;
  stack->top = -1;
  stack->items = (ScopeTable **)malloc(sizeof(ScopeTable *));

  return stack;
}

void push(ScopeStack *stack, ScopeTable *item) {
  if (stack->top == stack->size - 1) {
    stack->size++;
    stack->items = (ScopeTable **)realloc(stack->items, stack->size * sizeof(ScopeTable *));
  }
  
  stack->top++;
  stack->items[stack->top] = item;
}

void pop(ScopeStack *stack) {
  stack->top--;
}

void free_tree(Node *node) {
  if (node == NULL)
    return;

  free_tree(node->child);
  free_tree(node->sibling);

  free(node);
}

void free_line_list(LineList *lines) {
  free(lines->list);
  free(lines);
}

void free_symbol(Symbol *symbol) {
  free_line_list(symbol->lines);
  free(symbol);
}

void free_hash_table(HashTable *table) {
  for (int i = 0; i < table->size; i++) {
    if (table->items[i] != NULL)
      free_symbol(table->items[i]);
  }
  
  free(table->items);
  free(table);
}

void free_scope_table(ScopeTable *scope_table) {
  if (scope_table == NULL)
    return;

  free_hash_table(scope_table->table);
  free_scope_table(scope_table->child);
  free_scope_table(scope_table->sibling);
  free(scope_table);
}

void free_stack(ScopeStack *stack) {
  free(stack->items);
  free(stack);
}