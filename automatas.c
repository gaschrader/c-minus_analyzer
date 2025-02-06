#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int classify_char(char ch) {
  if (isdigit(ch)) {
    return 0;
  } else if (isalpha(ch)) {
    return 1;
  } else {
    return 2;
  }
}

//  algoritmo para rodar automatos
// dirigidos por tabela

int run_automata(char *string, int transition[][2], int accept[],
                 int num_states) {
  int state = 0;
  int ch;

  for (int i = 0; i < strlen(string); i++) {
    ch = classify_char(string[i]);

    // chegou em estado lixo ou transicao invalida
    if (ch > num_states || state == 2)
      return 0;

    state = transition[state][ch];
  }

  return accept[state];
}

//  automata 1: reconhece identificadores
//  tabela de transicao e aceitacao

int t_id[2][2] = {{2, 1}, {2, 1}};
int accept_id[2] = {0, 1};

//  automata 2: reconhece numeros
//  tabela de transicao e aceitacao

int t_num[2][2] = {{1, 2}, {1, 2}};
int accept_num[2] = {0, 1};