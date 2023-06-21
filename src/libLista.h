#ifndef _LIBlista_t_H
#define _LIBlista_t_H

#include <stdio.h>
#include <stdlib.h>

#include "libMetadado.h"

struct nodo_l {
	metadado_t *elemento; /* metadados do arquivo */
	struct nodo_l *prox;  /* ponteiro para o proximo   */
};
typedef struct nodo_l nodo_l_t;

struct lista {
	nodo_l_t *ini; /* ponteiro para o inicio da lista */
	int tamanho;   /* numero de elementos na lista    */
};
typedef struct lista lista_t;

/*
 * Cria uma lista vazia e a retorna, se falhar retorna NULL.
 */
lista_t *lista_cria();

/*
 * Remove todos os elementos da lista, libera espaco e retorna NULL.
 */
lista_t *lista_destroi(lista_t *l);

/*
 * Retorna 1 se a lista esta vazia e 0 caso contrario.
 */
int lista_vazia(lista_t *l);

/*
 * Retorna o tamanho da lista, isto eh, o numero de elementos presentes nela.
 */
int lista_tamanho(lista_t *l);

/*
 * Insere o elemento no inicio da lista.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
 */
int lista_insere_inicio(lista_t *l, metadado_t *elemento);

/*
 * Insere o elemento no final da lista.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
 */
int lista_insere_fim(lista_t *l, metadado_t *elemento);

/*
 * Remove o elemento do final da lista e o retorna em 'elemento'.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
 */

int lista_retira_fim(lista_t* l);

/*
 * Remove o elemento do inicio da lista e o retorna em 'elemento'.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
 */
int lista_retira_inicio(lista_t *l, metadado_t *elemento);

/**
 * Retorna 1 se o nome do arquivo existe na lista e 0 caso contrario.
 * Retorna em "metadado" o metadado com o mesmo nome do arquivo
 */
int lista_pertence(lista_t *l, char *membro);

/**
 * Ordena a lista pelas posições dos metadados e retorna a mesma. */
lista_t *lista_ordena_metadados(lista_t *l);

/*
 * Remove o elemento 'elemento' caso ele exista na lista.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
 * Se o elemento nao for encontrado na lista tambem retorna 0.
 */
int lista_retira_elemento(lista_t *l, metadado_t *elemento);

/*
 * Imprime a lista, do inicio ate o fim, este por ultimo, sem espaco no final.
 * Quando a lista estiver vazia, nao imprime nada, nem um \n.
 * Esta funcao eh somente para facilitar teus testes.
 * Normalmente ela nao existe nas implementacoes de um TAD lista.
 */
void lista_imprime(lista_t *l);

/**
 * Função que troca os elementos da lista e os reordena.
 */
void lista_troca(lista_t *l, metadado_t *file1, metadado_t *file2);

/**
 * Função que insere todos os metadados do diretório.
 */
void lista_insere_metadados(FILE *file, lista_t *l);

/*
 * Insere a lista no arquivo do inicio ate o fim.
 */
void lista_escreve_arquivo(FILE *arq, lista_t *l);

/**
 * Função que retrocede os dados necessários dos metadados posteriores a ele.
 * Dados necessários = posicao e localizacao.
 **/
void lista_retrocede_dados(nodo_l_t *nodo);

/**
 * Função que retorna o nodo da lista que possui o metadado com o nome especificado. Retorna NULL caso contrário.
*/
nodo_l_t *getNodo(lista_t* l,  char *nome);


/**
 * Função que retorna os metadados que estão dentro da lista se conter o mesmo nome especificado em "nome". Retorna NULL caso não encotnre.
 */
metadado_t *getMetadados(lista_t *l, char *nome);
#endif