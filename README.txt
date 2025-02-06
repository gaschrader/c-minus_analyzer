Código feito por Gabriel Schrader Vilas Boas.

Para compilar o código completo, utilizando o gcc, basta fazer:

gcc -o main main.c lexer.c parser.c semantic.c

Caso queira verificar possíveis erros, adicionar a flag -Wall ao final da linha acima.

Após compilado, basta rodar o executável da seguinte forma:

./main arquivo_texto -l -p -s

Em que:
  - arquivo_texto é o nome do .txt ou código que você deseja analisar, sendo obrigatório;
  - "-l" ou "-L" é a flag que imprime todos os tokens obtidos na análise léxica, que é opcional;
  - "-p" ou "-P" é a flag que imprime a árvore gerada na análise sintática, que é opcional;
  - "-s" ou "-S" é a flag que imprime a tabela de símbolos montada na análise semântica, que é opcional;
  - arquivo_texto precisa ser passado nessa exata ordem, enquanto que as outras flags podem ser chamadas na ordem desejada.
  *OBS*: Note que se nenhuma flag for passada, não haverá saída no terminal.