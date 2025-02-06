#ifndef PARSER_H
#define PARSER_H

#include "common.h"

// funções do analisador sintático

Node *parser();
void initialize_parser();
Token next_token(int look_ahead);
void advance_token();
Node *match(char *expected_token_type);
void print_syntatic_error(Token token);
void display_tree(Node *root);
void print_node(Node *node, int tab_count);
void indent_output(int tab_count);

// funções de Node

Node *create_node(char *value);
void insert_child(Node *parent, Node *child);

// funções da gramatica

Node *programa();
Node *decl_lista();
Node *decl();
Node *var_decl();
Node *fun_decl();
Node *tipo_especificador();
Node *params();
Node *param_lista();
Node *param();
Node *composto_decl();
Node *local_decl();
Node *statement_lista();
Node *statement();
Node *expressao_decl();
Node *selecao_decl();
Node *iteracao_decl();
Node *retorno_decl();
Node *expressao();
Node *var();
Node *simples_expressao();
Node *relacional();
Node *soma_expressao();
Node *soma();
Node *termo();
Node *mult();
Node *fator();
Node *ativacao();
Node *args();
Node *arg_lista();

#endif