#ifndef OPCOES_H_
#define OPCOES_H_

#include <stdio.h>
#include <stdlib.h>

#include "libLista.h"
/**
 * Remove o arquivo do archiver
*/
void remove_arquivo(char *vpp, char* fileName);


/**
 * Função temporária, forja um .vpp
 */
void forja(char *vppName, char *fileName);

/**
 * Função que lista todos os arquivos presentes no archiver.
 */
void lista_arquivos(char *vppName, char **fileNames);


/**
 * Imprime o manual de ajuda.
*/
void help();

/**
 * Função que substitui o arquivo antigo pelo novo de mesmo nome.
 */
void substitui(FILE *leitor, FILE *escritor, FILE *file, nodo_l_t *antigo, metadado_t *novo);


#endif