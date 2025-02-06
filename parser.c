#include "parser.h"

int parser_flag = FALSE;

Token curr_token;
int token_pos;

Node *parser() {
  initialize_parser();
  Node *root = programa();

  if (parser_flag)
    display_tree(root);

  return root;
}

void initialize_parser() {
  curr_token = next_token(0);
  token_pos = 0;
}

Token next_token(int look_ahead) {
  return tokens[token_pos + look_ahead];
}

void advance_token() {
  if (token_pos < num_tokens - 1)
    token_pos++;

  curr_token = next_token(0);
}

Node *match(char *expected_token_type) {
  Node *node = NULL;

  if (!strcmp(curr_token.type, expected_token_type)) {
    node = create_node(curr_token.value);
    node->line = curr_token.line;
    advance_token();
  } else {
    print_syntatic_error(curr_token);
  }

  return node;
}

void print_syntatic_error(Token token) {
  printf("ERRO SINTATICO: \"%s\" INVALIDO [linha: %d], COLUNA %d\n", token.value,
         token.line, token.col);
  exit(1);
}

void display_tree(Node *root) {
  printf("# ÁRVORE SINTÁTICA\n\n");
  print_node(root, 0);
}

void print_node(Node *node, int tab_count) {
  if (node == NULL)
    return;

  indent_output(tab_count);

  char *close_parenthesis = (tab_count == 0) ? ")" : "),";

  if (node->child) {
    printf("%s(\n", node->value);
    print_node(node->child, tab_count + 1);
    indent_output(tab_count);
    printf("%s\n", close_parenthesis);
  } else {
    if (node->sibling)
      printf("%s,\n", node->value);
    else
      printf("%s\n", node->value);
  }

  if (node->sibling)
    print_node(node->sibling, tab_count);
}

void indent_output(int tab_count) {
  char *tab = "  ";

  for (int i = 0; i < tab_count; i++)
    printf("%s", tab);
}

Node *create_node(char *value) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->value = value;
  node->line = 0;
  node->child = NULL;
  node->sibling = NULL;

  return node;
}

void insert_child(Node *parent, Node *child) {
  if (parent->child == NULL) {
    parent->child = child;
  } else {
    Node *curr_child = parent->child;

    while (curr_child->sibling != NULL)
      curr_child = curr_child->sibling;

    curr_child->sibling = child;
  }
}

Node *programa() {
  Node *node = create_node("programa");
  insert_child(node, decl_lista());

  return node;
}

Node *decl_lista() {
  Node *node = create_node("decl_lista");
  insert_child(node, decl());

  if (!strcmp(next_token(0).type, "int") ||
      !strcmp(next_token(0).type, "void"))
    insert_child(node, decl_lista());

  return node;
}

Node *decl() {
  Node *node = create_node("decl");

  if (!strcmp(next_token(1).type, "id")) {
    if (!strcmp(next_token(2).type, ";") || !strcmp(next_token(2).type, "[")) {
      insert_child(node, var_decl());
    } else if (!strcmp(next_token(2).type, "(")) {
      insert_child(node, fun_decl());
    }
  } else {
    print_syntatic_error(curr_token);
  }

  return node;
}

Node *var_decl() {
  Node *node = create_node("var_decl");
  insert_child(node, tipo_especificador());
  insert_child(node, match("id"));

  if (!strcmp(next_token(0).type, "[")) {
    insert_child(node, match("["));
    insert_child(node, match("num"));
    insert_child(node, match("]"));
  }

  insert_child(node, match(";"));

  return node;
}

Node *fun_decl() {
  Node *node = create_node("fun_decl");
  insert_child(node, tipo_especificador());
  insert_child(node, match("id"));
  insert_child(node, match("("));
  insert_child(node, params());
  insert_child(node, match(")"));
  insert_child(node, composto_decl());

  return node;
}

Node *tipo_especificador() {
  Node *node = create_node("tipo_especificador");

  if (!strcmp(next_token(0).type, "int") ||
      !strcmp(next_token(0).type, "void"))
    insert_child(node, match(next_token(0).type));
  else
    print_syntatic_error(curr_token);

  return node;
}

Node *params() {
  Node *node = create_node("params");

  if (!strcmp(next_token(0).type, "void"))
    insert_child(node, match("void"));
  else
    insert_child(node, param_lista());

  return node;
}

Node *param_lista() {
  Node *node = create_node("param_lista");
  insert_child(node, param());

  if (!strcmp(next_token(0).type, ",")) {
    insert_child(node, match(","));
    insert_child(node, param_lista());
  }

  return node;
}

Node *param() {
  Node *node = create_node("param");
  insert_child(node, tipo_especificador());
  insert_child(node, match("id"));

  if (!strcmp(next_token(0).type, "[")) {
    insert_child(node, match("["));
    insert_child(node, match("]"));
  }

  return node;
}

Node *composto_decl() {
  Node *node = create_node("composto_decl");
  insert_child(node, match("{"));
  insert_child(node, local_decl());
  insert_child(node, statement_lista());
  insert_child(node, match("}"));

  return node;
}

Node *local_decl() {
  Node *node = create_node("local_decl");

  if (!strcmp(next_token(0).type, "int") ||
      !strcmp(next_token(0).type, "void")) {
    insert_child(node, var_decl());
    insert_child(node, local_decl());
  }

  return node;
}

Node *statement_lista() {
  Node *node = create_node("statement_lista");

  if (!strcmp(next_token(0).type, "id") || !strcmp(next_token(0).type, "(") ||
      !strcmp(next_token(0).type, "num") || !strcmp(next_token(0).type, "{") ||
      !strcmp(next_token(0).type, "if") ||
      !strcmp(next_token(0).type, "while") ||
      !strcmp(next_token(0).type, "return")) {
    insert_child(node, statement());
    insert_child(node, statement_lista());
  }

  return node;
}

Node *statement() {
  Node *node = create_node("statement");

  if (!strcmp(next_token(0).type, "id") || !strcmp(next_token(0).type, "(") ||
      !strcmp(next_token(0).type, "num") || !strcmp(next_token(0).type, ";")) {
    insert_child(node, expressao_decl());
  } else if (!strcmp(next_token(0).type, "{")) {
    insert_child(node, composto_decl());
  } else if (!strcmp(next_token(0).type, "if")) {
    insert_child(node, selecao_decl());
  } else if (!strcmp(next_token(0).type, "while")) {
    insert_child(node, iteracao_decl());
  } else if (!strcmp(next_token(0).type, "return")) {
    insert_child(node, retorno_decl());
  } else {
    print_syntatic_error(curr_token);
  }

  return node;
}

Node *expressao_decl() {
  Node *node = create_node("expressao_decl");

  if (!strcmp(next_token(0).type, ";")) {
    insert_child(node, match(";"));
  } else {
    insert_child(node, expressao());
    insert_child(node, match(";"));
  }

  return node;
}

Node *selecao_decl() {
  Node *node = create_node("selecao_decl");
  insert_child(node, match("if"));
  insert_child(node, match("("));
  insert_child(node, expressao());
  insert_child(node, match(")"));
  insert_child(node, statement());

  if (!strcmp(next_token(0).type, "else")) {
    insert_child(node, match("else"));
    insert_child(node, statement());
  }

  return node;
}

Node *iteracao_decl() {
  Node *node = create_node("iteracao_decl");
  insert_child(node, match("while"));
  insert_child(node, match("("));
  insert_child(node, expressao());
  insert_child(node, match(")"));
  insert_child(node, statement());

  return node;
}

Node *retorno_decl() {
  Node *node = create_node("retorno_decl");
  insert_child(node, match("return"));

  if (!strcmp(next_token(0).type, "id") || !strcmp(next_token(0).type, "(") ||
      !strcmp(next_token(0).type, "num"))
    insert_child(node, expressao());

  insert_child(node, match(";"));

  return node;
}

Node *expressao() {
  Node *node = create_node("expressao");

  if (!strcmp(next_token(0).type, "id") && !strcmp(next_token(1).type, "=")) {
    insert_child(node, var());
    insert_child(node, match("="));
    insert_child(node, expressao());
  } else if (!strcmp(next_token(0).type, "id") &&
             !strcmp(next_token(1).type, "[")) {
    insert_child(node, var());
    insert_child(node, match("="));
    insert_child(node, expressao());
  } else {
    insert_child(node, simples_expressao());
  }

  return node;
}

Node *var() {
  Node *node = create_node("var");
  insert_child(node, match("id"));

  if (!strcmp(next_token(0).type, "[")) {
    insert_child(node, match("["));
    insert_child(node, expressao());
    insert_child(node, match("]"));
  }

  return node;
}

Node *simples_expressao() {
  Node *node = create_node("simples_expressao");
  insert_child(node, soma_expressao());

  if (!strcmp(next_token(0).type, "<=") || !strcmp(next_token(0).type, "<") ||
      !strcmp(next_token(0).type, ">") || !strcmp(next_token(0).type, ">=") ||
      !strcmp(next_token(0).type, "==") || !strcmp(next_token(0).type, "!=")) {
    insert_child(node, relacional());
    insert_child(node, soma_expressao());
  }

  return node;
}

Node *relacional() {
  Node *node = create_node("relacional");
  insert_child(node, match(curr_token.type));

  return node;
}

Node *soma_expressao() {
  Node *node = create_node("soma_expressao");
  insert_child(node, termo());

  if (!strcmp(next_token(0).type, "-") || !strcmp(next_token(0).type, "+")) {
    insert_child(node, soma());
    insert_child(node, soma_expressao());
  }

  return node;
}

Node *soma() {
  Node *node = create_node("soma");
  insert_child(node, match(curr_token.type));

  return node;
}

Node *termo() {
  Node *node = create_node("termo");
  insert_child(node, fator());

  if (!strcmp(next_token(0).type, "/") || !strcmp(next_token(0).type, "*")) {
    insert_child(node, mult());
    insert_child(node, termo());
  }

  return node;
}

Node *mult() {
  Node *node = create_node("mult");
  insert_child(node, match(curr_token.type));

  return node;
}

Node *fator() {
  Node *node = create_node("fator");

  if (!strcmp(next_token(0).type, "id")) {
    if (!strcmp(next_token(1).type, "(")) {
      insert_child(node, ativacao());
    } else {
      insert_child(node, var());
    }
  } else if (!strcmp(next_token(0).type, "(")) {
    insert_child(node, match("("));
    insert_child(node, expressao());
    insert_child(node, match(")"));
  } else if (!strcmp(next_token(0).type, "num")) {
    insert_child(node, match("num"));
  } else {
    print_syntatic_error(curr_token);
  }

  return node;
}

Node *ativacao() {
  Node *node = create_node("ativacao");
  insert_child(node, match("id"));
  insert_child(node, match("("));
  insert_child(node, args());
  insert_child(node, match(")"));

  return node;
}

Node *args() {
  Node *node = create_node("args");

  if (!strcmp(next_token(0).type, "id")) {
    insert_child(node, arg_lista());
  }

  return node;
}

Node *arg_lista() {
  Node *node = create_node("arg_lista");
  insert_child(node, expressao());

  if (!strcmp(next_token(0).type, ",")) {
    insert_child(node, match(","));
    insert_child(node, arg_lista());
  }

  return node;
}
