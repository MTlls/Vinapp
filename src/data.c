#include "data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "libLista.h"
#include "libMetadado.h"
#include "libUtils.h"
#include "saltos.h"

#define kB 1024
#define BLOCO_BYTE 341

/**
 * Função que transporta os bytes de um arquivo a outro.
 * Essa função Lê e escreve a partir da posição das streams entregues como argumento.
 * Transporta 1kB de cada vez.
 */
void transporta_bytes(FILE *destino, FILE *origem) {
	char buffer[kB];
	int bytesLidos = 0;

	while((bytesLidos = fread(buffer, 1, kB, origem)) == kB) {
		fwrite(buffer, 1, kB, destino);
	}
	fwrite(buffer, 1, bytesLidos, destino);
}

/**
 * Essa função reescreve o arquivo a partir do ponto atual, pegando dados de um ponto x do mesmo arquivo.
 * autoReescrita pois ele se reescreve, sobrescrevendo dados que agora são lixo.
 */
void autoReescrita(FILE *destino, int x) {
	char bufferLeitura[1024];
	int lidos, posEscrita = ftell(destino);

	// Captura-se 1024B do ponto x
	fseek(destino, x, SEEK_SET);

	while(feof(destino)) {
		// Lê o buffer e retorna os numero de bytes lidos
		lidos = fread(bufferLeitura, 1, 1024, destino);
		// Incrementa-se a posição de leitura
		x += lidos;

		// É escrito "lidos" bytes em destino
		fseek(destino, posEscrita, SEEK_SET);
		fwrite(bufferLeitura, lidos, 1, destino);
		// Incrementa-se a posição de escrita
		posEscrita += lidos;
		fseek(destino, x, SEEK_SET);
	}
}

/**
 * Função que realiza a sobrescrita do diretório com o novo arquivo e
 * insere a nova lista de metadados.
 */
void insere_atualiza_diretorio(FILE *vpp, FILE *file, lista_t *l) {
	// Salta para a área do diretório.
	seekDiretorio(vpp);

	// Escreve os dados.
	transporta_bytes(vpp, file);

	// Atualiza o necessário sobre o diretório.
	atualizaDir(vpp, l);
}

int fileExists(FILE *vpp, char *fileName, lista_t *l, nodo_l_t **info) {
	char nome[256];

	// Formata o nome que veio do terminal.
	strcpy(nome, fileName);
	retiraEspacos(nome);

	// Salta-se até o diretório.
	seekDiretorio(vpp);

	// Insere todos os metadados na lista;
	lista_insere_metadados(vpp, l);

	// lista_pertence procura o nome dentro da lista.
	if(lista_pertence(l, nome) == 0) {
		return 0;
	};
	// Alterado o ponteiro do nodo, agora apontando o metadado que pertence.
	*info = getNodo(l, nome);
	return 1;
}

// /**
//  * Função que shifta tudo que há a direita do alvo para a esquerda.
//  * Sendo a quantidade de shifts o tamanho do alvo.
//  */
// void shiftEsq(FILE *file, int localDespejo, int tamAlvo) {
// 	char buffer[kB];
// 	int bytesRestantes = tamAlvo;
// 	int pedacos = tamAlvo / kB;

// 	fseek(file, localDespejo + tamAlvo, SEEK_SET);

// 	for(; pedacos > 0; pedacos--) {
// 		fread(buffer, kB, 1, origem);
// 		bytesRestantes -= kB;
// 		fwrite(buffer, kB, 1, destino);
// 	}

// 	fread(buffer, bytesRestantes, 1, origem);
// 	fwrite(buffer, bytesRestantes, 1, destino);
// }

/**
 * Função que imprime as permissões em STDIN.
 */
void imprimePermissoes(int mode) {
	int i, c;
	char permissoes[3] = {'x', 'w', 'r'};

	// Todos os arquivos são... arquivos, não diretórios.
	putc('-', stdout);
	// Captura todos os 9 bits da representação octal de st_mode.
	for(i = 8; i >= 0; i--) {
		// Verifica o i-ésimo bit de st_mode e o imprime.
		c = (mode & (1 << i)) ? permissoes[(i % 3)] : '-';
		printf("%c", c);
	}
}

/**
 * Função realiza um shift a direita.
 * Recebe as streams já posicionadas no fim da area de leitura e de escrita do shitf, mais a quantidade de bytes que será shiftado.
 */
void shift_direita(FILE *destino, FILE *origem, int qntdDados) {
	int bytesRestantes = qntdDados;
	int pos_escritor = 0, pos_leitor = 0;
	char buffer[kB];

	//
	int ini_leitor = ftell(origem);
	int ini_escritor = ftell(destino);

	if(bytesRestantes >= kB) {
		pos_leitor = fseek(origem, -kB, SEEK_CUR);
		pos_escritor = fseek(destino, -kB, SEEK_CUR);
	}

	for(; bytesRestantes >= kB; bytesRestantes -= kB) {
		fread(buffer, 1, kB, origem);
		fwrite(buffer, 1, kB, destino);
		pos_leitor = fseek(origem, pos_leitor - kB, SEEK_SET);
		pos_escritor = fseek(destino, pos_escritor - kB, SEEK_SET);
	}

	fseek(origem, ini_leitor - qntdDados, SEEK_SET);
	fseek(destino, ini_escritor - qntdDados, SEEK_SET);
	fread(buffer, 1, bytesRestantes, origem);
	fwrite(buffer, 1, bytesRestantes, destino);
}

/**
 * Transporta N bytes da origem até o destino.
 */
void transporta_n_bytes(FILE *destino, FILE *origem, int qntDados) {
	int bytesRestantes = qntDados;
	char buffer[kB];

	for(; bytesRestantes >= kB; bytesRestantes -= kB) {
		fread(buffer, 1, kB, origem);
		fwrite(buffer, 1, kB, destino);
	}

	fread(buffer, 1, bytesRestantes, origem);
	fwrite(buffer, 1, bytesRestantes, destino);
}