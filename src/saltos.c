#include <stdio.h>
#include <stdlib.h>

#include "saltos.h"
#include "libLista.h"

/**
 * Lê a posição do diretório e pula para a posição.
 * OBS: Offset é necessário estar atualziado dentro do arquivo para realizar isso!
 */
void seekDiretorio(FILE *file) {
	int pos = 0;
	rewind(file);
	fread(&pos, sizeof(int), 1, file);
	fseek(file, pos, SEEK_SET);
}

/**
 * Função que atualiza a área do diretório com a lista enviada.
 */
void atualizaDir(FILE *vpp, lista_t *l) {
	seekDiretorio(vpp);
	lista_escreve_arquivo(vpp, l);
}

/**
 * Função que atualiza o offset (area onde incia o diretorio).
 */
void atualizaOffset(FILE *file, int offset) {
	rewind(file);
	fwrite(&offset, sizeof(int), 1, file);
}

/**
 * Função que retorna o offset (area onde incia o diretorio).
 */
int getOffset(FILE *file) {
	int offset = 0;
	rewind(file);
	
	if(fread(&offset, sizeof(int), 1, file) == 0){
		fprintf(stderr, "Erro ao ler o offset.\n");
		exit(1);
	}
	return offset;
}