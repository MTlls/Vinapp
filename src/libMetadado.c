#include "libMetadado.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

/**
 * Essa função captura os metadados recebidos do arquivo
 * e copia alguns deles dentro da struct metadado_t.
 * Retorna um ponteiro para a struct metadado_t criada.
 */
metadado_t *getMetaDados(struct stat status, char *caminhoArquivo) {
	metadado_t *metadados;
	char data_temp[16];

	if((metadados = aloca_metadado()) == NULL) {
		return NULL;
	}

	formataNome((metadados->nome), caminhoArquivo);
	formataCaminho((metadados->caminho), caminhoArquivo);

	// Converte o horário da ultima modificação (que estava em Epoch Time) para um horário local.
	strftime(data_temp, 17, "%F %R", localtime(&status.st_mtim.tv_sec));
	// ctime retorna sempre um \n no final e tem um tamanho constante, então...
	data_temp[24] = '\0';
	strncpy(metadados->data_modificacao, data_temp, 17);

	if(!(metadados->data_modificacao) || !(metadados->nome)) {
		return NULL;
	}

	// Inserido o tipo e modo do arquivo, será usado depois para recriação do mesmo.
	metadados->mode = status.st_mode;
	metadados->uid = status.st_uid;
	metadados->tamanho = status.st_size;

	return metadados;
}

/**
 * Função que aloca um metadado genérico.
 * Retorna NULL caso ocorra um erro.
 */
metadado_t *aloca_metadado() {
	metadado_t *metadado;
	if(!(metadado = (metadado_t *)calloc(1, sizeof(metadado_t)))) {
		return NULL;
	}
	return metadado;
}

/**
 * Função que desaloca o metadado.
 */
void desaloca_metadado(metadado_t *metadado) {
	free(metadado);
}

/**
 * Imprime um nodo no formato
 * -rw------- telles/telles   584 2023-06-14 02:47 t3/makefile
 */
void metadado_imprime(metadado_t *metadado) {
	imprimePermissoes(metadado->mode);
	printf(" %s", getUsername(metadado->uid));
	printf(" %5d", metadado->tamanho);
	printf(" %s", metadado->data_modificacao);
	printf(" %s", metadado->caminho);
	// printf(" | ");
	// printf(" %5d", metadado->localizacao);
	// printf(" %d", metadado->posicao);
	// printf(" %s", metadado->nome);
	printf("\n");
}

char *getUsername(uid_t uid) {
	return getpwuid(uid)->pw_name;
}

/**
 * Função que formata os uma string como um caminho relativo.
 */
void formataCaminho(char *dest, char *src) {
	// Independente de ser um caminho relativo ou não, é necessário com que o metadado de localização seja relativo ao diretório do archive. Será inserido um "./" no começo da string de localização.
	dest[0] = '.';

	// Verifica se começa com '/' ou não
	if(src[0] != '/')
		dest[1] = '/';

	// concatena ./ + str
	strcat(dest, src);
}

/**
 * Função que apenas pega a ultima string após o '/'.
 */
void formataNome(char *dest, char *src) {
	int i;
	// Aponta para o primeiro char depois de '/'
	char *ponteiroNome;
	for(i = strlen(src); i >= 0 && src[i] != '/'; i--)
		ponteiroNome = src + i;

	strcpy(dest, ponteiroNome);
	// Autoexplicativo.
	retiraEspacos(dest);
}

/**
 * Função que retira os espaços da sting.
 */
void retiraEspacos(char *str) {
	int aux = 0, tam = strlen(str), i;
	for(i = 0; i < tam; i++) {
		if(str[i] != ' ')
			str[aux++] = str[i];
	}
	str[aux] = '\0';
}