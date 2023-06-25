#ifndef SALTOS_H_
#define SALTOS_H_

#include <stdio.h>
#include <stdlib.h>

#include "libLista.h"

/**
 * Lê a posição do diretório e pula para a posição.
 */
void seekDiretorio(FILE *file);

/**
 * Função que atualiza a área do diretório com a lista enviada.
 */
void atualizaDir(FILE *vpp, lista_t *l);

/**
 * Função que atualiza o offset (area onde incia o diretorio).
 */
void atualizaOffset(FILE *vpp, int offset);

/**
 * Função que retorna o offset (area onde incia o diretorio).
 */
int getOffset(FILE *file);

#endif