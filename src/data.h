#ifndef DATA_H_
#define DATA_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "libLista.h"
#include "libMetadado.h"

/**
 * Função que transporta os bytes de um arquivo a outro.
 * Transporta 1kB de cada vez.
 */
void transporta_bytes(FILE *destino, FILE *origem);

/**
 * Essa função reescreve o arquivo a partir do ponto atual, pegando dados de um ponto x do mesmo arquivo.
 * autoReescrita pois ele se reescreve, sobrescrevendo dados que agora são lixo.
 */
void autoReescrita(FILE *destino, int x);

/**
 * Função que verifica se há tal arquivo no archiver, retorna 1 se caso exista o arquivo, 0 se não exista.
 * Para facilitar a futura manipulação, é retornado o nodo do metadado alvo.
 * Caso retorne 0, o metadado retornado é NULL.
 */
int fileExists(FILE *vpp, char *fileName, lista_t *l, nodo_l_t **info);

/**
 * Função que atualiza a área do diretório com a lista enviada.
 */
void atualizaDir(FILE *vpp, lista_t *l);

/**
 * Função que realiza a sobrescrita do diretório com o novo arquivo e
 * insere a nova lista de metadados.
 */
void insere_atualiza_diretorio(FILE *vpp, FILE *file, lista_t *l);

/**
 * Função realiza um shift a direita.
 * Recebe a posição da stream do inicio e do fim da área shiftada, mais a quantidade de bytes que será shiftado.
 */
void shift_direita(FILE *destino, FILE *origem, int qntdDados);

/**
 * Transporta N bytes da origem até o destino.
*/
void transporta_n_bytes(FILE *destino, FILE *origem, int qntDados);
#endif