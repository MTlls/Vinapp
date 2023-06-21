#ifndef _LIBMETADADO_H_
#define _LIBMETADADO_H_

#include <sys/stat.h>
#include <stdio.h>

/*Funções que manipulam as structs usadas para montar uma lista básica*/
struct metadado {
	unsigned int uid;           // User ID
	unsigned int localizacao;   // local do arquivo dentro do archiver
	unsigned int tamanho;       // Tamanho do arquivo.
	unsigned int mode;          // modo do arquivo
	unsigned short posicao;     // Posição em que o arquivo se encontra, 0 a no máximo 65536.
	char data_modificacao[17];  // Data em que o arquivo foi modificado. Armazenado em string para menor guardar menor quantidade de bytes.
	char *nome;                 // Nome (sem espaços), máximo 80 caracteres.
	char *caminho;              // Caminho do arquivo.
};
typedef struct metadado metadado_t;

/**
 * Essa função captura os metadados recebidos do arquivo
 * e copia alguns deles dentro da struct metadado_t.
 * Retorna um ponteiro para a struct metadado_t criada.
 */
metadado_t *getStats(struct stat status, char *caminhoArquivo);

/**
 * Função que imprime as permissões em STDOUT.
 */
void imprimePermissoes(int mode);

/**
 * Função que aloca um metadado genérico.
 * Ainda é necessário alocar memória para os dois ponteiros de char membros.
 * Retorna NULL caso ocorra um erro.
 */
metadado_t *aloca_metadado();

/**
 * Função que desaloca o metadado.
 */
void desaloca_metadado(metadado_t *metadado);

/*
 * Imprime um nodo e seus dados.
 */
void metadado_imprime(metadado_t *metadado);

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
 * Lê o uid e retorna o username respectivo.
*/
char *getUsername(unsigned int uid);

/**
 * Função que lê uma string até o byte nulo.
 * Aloca espaço suficiente para a string e a retorna dentro do parâmetro incluindo o byte nulo '\0'.
 * Em caso de erro a string retornará nula.
*/
void leString(FILE *file, char **str);

/**
 * Escreve o metadado no arquivo nesta determinada sequência:
 * localicação, tamanho, modo, posição, data_modificacao, nome, caminho.
*/
void escreve_metadado(FILE *file, metadado_t *metadado);

/**
 * Le o metadado no arquivo nesta determinada sequência:
 * localicação, tamanho, modo, posição, data_modificacao, nome, caminho.
 * Retorna 0 se não conseguir ler nada no primeiro membro. 1 caso contraŕio.
*/
int le_metadado(FILE *file, metadado_t *metadado);

#endif