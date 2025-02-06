// Nome: Gabriel Schrader Vilas Boas
// RA: 150981

#include "lexer.h"
#include "parser.h"
#include "semantic.h"

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 5) {
    printf("Numero de argumentos do programa está incorreto. Encerrando o "
           "programa.\n");
    return 0;
  }

  // verifica se as flags são invocadas
  for (int i = 2; i < argc; i++) {
    if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "-L")) {
      lexer_flag = TRUE;
    } else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "-P")) {
      parser_flag = TRUE;
    } else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "-S")) {
      semantic_flag = TRUE;
    } else {
      printf("Argumento %s inválido. Encerrando o programa.\n", argv[i]);
      exit(1);
    }
  }

  FILE *file;
  file = fopen(argv[1], "r");

  if (file == NULL) {
    printf("Erro ao abrir o arquivo. Encerrando o programa.\n");
    exit(1);
  }

  Token *list_of_tokens = lexer(file);
  Node *root = parser(list_of_tokens);
  build_table(root);
  semantic_verification(root);

  return 0;
}