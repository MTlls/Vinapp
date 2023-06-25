#include "libMetadado.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "libUtils.h"
/**
 * Essa função captura a struct stat recebida do arquivo
 * e copia alguns metadados dentro da struct metadado_t.
 * Retorna um ponteiro para a struct metadado_t criada. NULL caso contrário.
 */
metadado_t *getStats(struct stat status, char *caminhoArquivo) {
	metadado_t *metadados;
	// tamanho máximo de um nome.
	char nomeTemp[256];
	char data_temp[17];

	if((metadados = aloca_metadado()) == NULL) {
		return NULL;
	}

	// Variável temporária, para obtermos o tamanho do nome do arquivo.
	formataNome(nomeTemp, caminhoArquivo);

	// Devido ao escopo e a necessidade de diminuir a memória, é necessário realocá-los com o tamanho da string lida dentro desta função.
	if(!(metadados->caminho = (char *)realloc(metadados->caminho, sizeof(char) * (strlen(caminhoArquivo) + 3)))) {
		return NULL;
	} else if(!(metadados->nome = (char *)realloc(metadados->nome, sizeof(char) * (strlen(nomeTemp) + 1)))) {
		return NULL;
	}

	// Converte o horário da ultima modificação (que estava em Epoch Time) para um horário local.
	strftime(data_temp, 17, "%F %R", localtime(&status.st_mtime));
	// ctime retorna sempre um \n no final e tem um tamanho constante, então...
	data_temp[16] = '\0';
	strncpy(metadados->data_modificacao, data_temp, 17);

	if(!(metadados->data_modificacao)) {
		return NULL;
	}

	// No máximo o tamanho do caminho é (./ + (path))
	strcpy(metadados->nome, nomeTemp);
	formataCaminho(metadados->caminho, caminhoArquivo);

	// Inserido o tipo e modo do arquivo, será usado depois para recriação do mesmo.
	metadados->permissoes = status.st_mode & 0777;  // Está em octal!
	metadados->uid = status.st_uid;
	metadados->tamanho = status.st_size;

	return metadados;
}

/**
 * Função que aloca um metadado genérico. Ainda é necessário realocar a memória necessária para os dois ponteiros de char membros.
 * Retorna NULL caso ocorra um erro.
 */
metadado_t *aloca_metadado() {
	metadado_t *metadado;
	if(!(metadado = (metadado_t *)calloc(1, sizeof(metadado_t)))) {
		return NULL;
	}
	// Para pelo menos have ruma referência, aloca-se o tamanho de 1 char.
	else if(!(metadado->nome = (char *)malloc(sizeof(char)))) {
		return NULL;
	} else if(!(metadado->caminho = (char *)malloc(sizeof(char)))) {
		return NULL;
	}
	return metadado;
}

/**
 * Função que desaloca o metadado.
 */
void desaloca_metadado(metadado_t *metadado) {
	free(metadado->caminho);
	free(metadado->nome);
	free(metadado);
}

/**
 * Imprime um nodo no formato
 * permissoes uid tamanho data caminho posicao
 */
void metadado_imprime(metadado_t *metadado) {
	imprimePermissoes(metadado->permissoes);
	printf(" %s %7d %s %s\n", getUsername(metadado->uid), metadado->tamanho, metadado->data_modificacao, metadado->caminho);

	printf("localizacao: %d\n nome: %s\n posicao: %d", metadado->localizacao, metadado->nome, metadado->posicao);
	printf("\n");
}

char *getUsername(unsigned int uid) {
	return getpwuid(uid)->pw_name;
}

void leString(FILE *file, char **str) {
	// charLido será atribuido definitivamente no laço
	char charLido = '\n';
	char temp[4096];
	int tamStr = 0;

	while(charLido != '\0') {
		// Caso de erro (não consiga ler mais nada)
		if(fread(&charLido, 1, 1, file) == 0) {
			str = NULL;
			return;
		}
		// Caso leia um byte/char
		else {
			temp[tamStr++] = charLido;
		}
	}
	*str = (char *)realloc(*str, sizeof(char) * tamStr);
	strcpy(*str, temp);
}

void escreve_metadado(FILE *file, metadado_t *metadado) {
	fwrite(&metadado->uid, 1, sizeof(unsigned int), file);
	fwrite(&metadado->localizacao, 1, sizeof(unsigned int), file);
	fwrite(&metadado->tamanho, 1, sizeof(unsigned int), file);
	fwrite(&metadado->permissoes, 1, sizeof(unsigned int), file);
	fwrite(&metadado->posicao, 1, sizeof(unsigned short), file);
	// data_modificacao[16], por isso o 16.
	fwrite(metadado->data_modificacao, 1, sizeof(char) * 16, file);
	fwrite(metadado->nome, 1, sizeof(char) * (strlen(metadado->nome) + 1), file);
	fwrite(metadado->caminho, 1, sizeof(char) * (strlen(metadado->caminho) + 1), file);
}

int le_metadado(FILE *file, metadado_t *metadados) {
	// Caso esteja no final do arquivo.
	if(fread(&metadados->uid, 1, sizeof(unsigned int), file) == 0)
		return 0;
	fread(&metadados->localizacao, 1, sizeof(unsigned int), file);
	fread(&metadados->tamanho, 1, sizeof(unsigned int), file);
	fread(&metadados->permissoes, 1, sizeof(unsigned int), file);
	fread(&metadados->posicao, 1, sizeof(unsigned short), file);
	fread(metadados->data_modificacao, 1, sizeof(char) * 16, file);
	leString(file, &metadados->nome);
	leString(file, &metadados->caminho);

	if(!(metadados->nome) || !(metadados->caminho)) {
		fprintf(stderr, "Erro ao alocar strings do metadado.\n");
		exit(1);
	}

	return 1;
}

void substitui_metadados(metadado_t *a, metadado_t *b){
		strcpy(a->data_modificacao, b->data_modificacao);
		a->tamanho = b->tamanho;
		a->permissoes = b->permissoes;
		a->uid = b->uid;
}

int podeAtualizar(metadado_t *inserido, metadado_t *candidato) {
	char *recente = mais_recente(inserido->data_modificacao, candidato->data_modificacao);
	// Verifica se as datas sao a mesma.
	if(strcmp(inserido->data_modificacao, candidato->data_modificacao) == 0){
		return 0;
	}

	// Verifica se o mais recente é o candidato.
	if(strcmp(recente, candidato->data_modificacao) == 0){
		return 1;
	}
	// Caso não seja, não é possível atualizar.
	return 0;
}