#ifndef OPCOES_H_
#define OPCOES_H_

#include <stdio.h>
#include <stdlib.h>

#include "libLista.h"

enum tipo_insercao{
    OPTION_I, // Opcao -a, insere ou atualiza com base na data de modificacao.
    OPTION_A  // Opcao -i, insere ou atualiza independente de data.
};
typedef enum tipo_insercao tipo_insercao_t;

/**
 * Remove o arquivo do archiver
*/
void remove_arquivo(char *vpp, char* fileName);


/**
 * Função temporária, forja um .vpp
 */
void insercao(char *vpp_name, char *fileName, tipo_insercao_t modo);

/**
 * Função que lista todos os arquivos presentes no archiver.
 */
void lista_arquivos(char *vpp_name, char **fileNames);


/**
 * Imprime o manual de ajuda.
*/
void help();

/**
 * Função que substitui o arquivo antigo pelo novo de mesmo nome.
 */
void substitui(FILE *leitor, FILE *escritor, FILE *file, nodo_l_t *antigo, metadado_t *novo);

/**
 * Função que move o membro indicado para imediatamente depois do target/alvo existente em archive.
*/
void movimenta(char *vpp_name, char *target_name, char *member_name);
#endif