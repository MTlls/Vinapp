#ifndef UTILS_H_
#define UTILS_H_

/**
 * Função que formata os uma string como um caminho relativo.
 * Retorna a string formatada.
 */
void formataCaminho(char *dest, char *src);

/**
 * Função que apenas pega a ultima string após o '/'.
 */
void formataNome(char *dest, char *src);

/**
 * Função que retira os espaços da string.
 */
void retiraEspacos(char *str);

/**
 * Função útil para printar o help, separa em colunas o print.
 */
void imprime_coluna(int recuo, int tamanho_coluna, char *texto);

/**
 * Função que retorna a data mais recente.
 * Recebe duas strings no formato "YYYY-MM-DD hh:mm".
 * Retorna NULL caso as duas datas sejam iguais.
*/
char* mais_recente(char *data_a, char *data_b);
#endif