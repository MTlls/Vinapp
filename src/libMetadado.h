#ifndef _LIBMETADADO_H_
#define _LIBMETADADO_H_

#include <sys/stat.h>

/*Funções que manipulam as structs usadas para montar uma lista básica*/
struct metadado {
	char nome[256];             // Nome (sem espaços), máximo 80 caracteres.
	char caminho[4096];         // Caminho do arquivo.
	unsigned int uid;           // User ID
	unsigned int localizacao;   // local do arquivo dentro do archiver
	unsigned int tamanho;       // Tamanho do arquivo.
	unsigned int mode;          // modo do arquivo
	unsigned short posicao;     // Posição em que o arquivo se encontra, 0 a no máximo 65536.
	char data_modificacao[16];  // Data em que o arquivo foi modificado. Armazenado em string para menor guardar menor quantidade de bytes.
};
typedef struct metadado metadado_t;

/**
 * Essa função captura os metadados recebidos do arquivo
 * e copia alguns deles dentro da struct metadado_t.
 * Retorna um ponteiro para a struct metadado_t criada.
 */
metadado_t *getMetaDados(struct stat status, char *caminhoArquivo);

/**
 * Função que imprime as permissões em STDOUT.
*/
void imprimePermissoes(int mode);

/**
 * Função que aloca um metadado genérico.
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
char *getUsername(uid_t uid);

#endif