#ifndef OPCOES_H_
#define OPCOES_H_

#include <stdio.h>
#include <stdlib.h>

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
void lista_arquivos(char *vppName);

#endif