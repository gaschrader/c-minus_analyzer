#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "common.h"

#define HASH_TABLE_SIZE 101

// estruturas

typedef struct {
  int size;
  int *list;
} LineList;

typedef struct {
  char *name;
  char *scope_name;
  char *id_type;
  char *data_type;
  LineList *lines;
} Symbol;

typedef struct {
  int size;
  Symbol **items;
} HashTable;

typedef struct ScopeTable {
  char *name;
  HashTable *table;
  struct ScopeTable *parent;
  struct ScopeTable *child;
  struct ScopeTable *sibling;
} ScopeTable;

typedef struct {
  ScopeTable **items;
  int top;
  int size;
} ScopeStack;

// variáveis globais

extern ScopeStack *stack;
extern ScopeTable *scope_table;

// funções do analisador semântico

void build_table(Node *node);
void semantic_verification(Node *root);
void initialize_semantic();
void create_full_table(Node *node);
void handle_declaration(int decl_type, Node *node, ScopeTable *stack_head, HashTable *curr_table);
void set_scope_action(ScopeTable *stack_head, Node *node);
void check_parent_table(ScopeTable *stack_head, char *key, int line);
void add_new_scope(ScopeTable *stack_head, char *scope_name);
void display_table();
void print_scope_table(ScopeTable *head);
void print_lines(LineList *lines);
void verify_tree(Node *node);
void verify_var_type(Node *node);
void verify_func_return(Node *node);
void verify_return(Node *node);
void free_tree(Node *node);
void print_semantic_error(char *message, int line);

// funções de LineList

void add_line(LineList *list, int value);
void free_line_list(LineList *list);

// funções de Symbol

Symbol *new_symbol(char *name, char *scope_name, char *id_type, char *data_type);
void free_symbol(Symbol *symbol);

// funções de HashTable

HashTable *create_table(int size);
void insert(HashTable *table, Symbol *symbol);
unsigned int hash(const char *key, int size);
Symbol *find_symbol(HashTable *table, char *name);
void free_hash_table(HashTable *table);

// funções de ScopeTable e ScopeStack

ScopeTable *create_scope_table(char *name);
void free_scope_table(ScopeTable *scope_table);
ScopeStack *create_stack(int size);
void push(ScopeStack *stack, ScopeTable *item);
void pop(ScopeStack *stack);
void free_stack(ScopeStack *stack);

#endif