#include "opcoes.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "data.h"
#include "libLista.h"
#include "saltos.h"

/**
 * Função que remove o arquivo indicado.
 */
void remove_arquivo(char *vppName, char *fileName) {
	// vpp_adiantado é o vpp, só que a frente do ponto de leitura do vpp.
	// Será usado para prevenir fseeks seguidos.
	FILE *vpp, *vpp_adiantado;
	lista_t *lista = NULL;

	// Para ficar mais legível.
	nodo_l_t *nodo_alvo = NULL;
	metadado_t *metadado_alvo = NULL;
	int inicio, fim, file_size, l_tamanho;

	if(!(vpp = fopen(vppName, "r+"))) {
		fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", fileName);
		exit(1);
	}
	// Aloca espaço para a lista.
	else if((lista = lista_cria()) == NULL) {
		fprintf(stderr, "Erro ao iniciar a lista.\n");
		fclose(vpp);
		exit(1);
	} else if(fileExists(vpp, fileName, lista, &nodo_alvo) == 0) {
		fprintf(stderr, "Erro: Arquivo inexistente.\n");
		lista_destroi(lista);
		fclose(vpp);
		exit(1);
	}

	metadado_alvo = nodo_alvo->elemento;
	l_tamanho = lista_tamanho(lista);

	// Se só há um metadado e iremos removê-lo, é truncado o arquivo
	// para ter apenas o offset como dado.
	if(l_tamanho == 1) {
		atualizaOffset(vpp, sizeof(int));
		truncate(vppName, sizeof(int));
		lista_destroi(lista);
		fclose(vpp);
		return;
	}

	// Localizacao e final do arquivo alvo
	inicio = metadado_alvo->localizacao;
	file_size = metadado_alvo->tamanho;
	fim = metadado_alvo->localizacao + metadado_alvo->tamanho;

	// Retrocede a posicao de todos os metadados posteriores.
	lista_retrocede_dados(nodo_alvo);
	// Retira o metadado alvo.
	lista_retira_elemento(lista, metadado_alvo);
	// Escreve a lista no arquivo.
	lista_escreve_arquivo(vpp, lista);

	// Exclui o último metadado que está sobrando.
	truncate(vppName, ftell(vpp));

	// Será aberto mais outro FILE* para a leitura simultânea do mesmo arquivo.
	// Só será necessário ler essa stream, por isso o modo "r".
	if(!(vpp_adiantado = fopen(vppName, "r"))) {
		fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", fileName);
		fclose(vpp);
		exit(1);
	}

	// Nova posição do offset é o offset antigo-tamanho do alvo
	atualizaOffset(vpp, getOffset(vpp) - file_size);

	// Salta-se para a localizacao do arquivo alvo.
	fseek(vpp, inicio, SEEK_SET);

	// Salta-se para a localizacao do final do arquivo alvo.
	fseek(vpp_adiantado, fim, SEEK_SET);

	// Transporta tudo que está após o final do alvo para a antiga localização do alvo
	transportaBytes(vpp, vpp_adiantado);

	// Exclui o lixo que estava após o shift de dados.
	truncate(vppName, ftell(vpp));
	lista_destroi(lista);

	printf("Removido!\n");

	if((lista = lista_cria()) == NULL) {
		fprintf(stderr, "Erro ao iniciar a lista.\n");
		exit(1);
	}
	// Insere todos os metadados na lista e retira o metadado alvo.
	lista_insere_metadados(vpp, lista);
	lista_imprime(lista);
	lista_destroi(lista);
	fclose(vpp);
	fclose(vpp_adiantado);
}

/**
 * Função temporária, forja um .vpp
 */
void forja(char *vppName, char *fileName) {
	FILE *vpp, *file;
	lista_t *lista;
	metadado_t *metadados;
	int offset = 0, size = 0;
	struct stat info;

	stat(fileName, &info);

	size = info.st_size;

	// Cria a lista.
	if((lista = lista_cria()) == NULL) {
		fprintf(stderr, "Erro ao iniciar a lista.\n");
		exit(1);
	}

	if(!(file = fopen(fileName, "r"))) {
		fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", fileName);
		lista_destroi(lista);
		exit(1);
	}

	// Captura os metadados do arquivo.
	if((metadados = getStats(info, fileName)) == NULL) {
		fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", fileName);
		lista_destroi(lista);
		fclose(file);
		exit(1);
	}

	// Verifica-se primeiro se o destino está acessível
	// caso esteja, o abre-se o arquivo no modo r+, economizando leitura dos
	// arquivos já inseridos.
	if(access(vppName, F_OK) == 0) {
		if(!(vpp = fopen(vppName, "r+"))) {
			fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", vppName);
			exit(1);
		}

		// Captura a posição que começa o diretório
		fread(&offset, sizeof(int), 1, vpp);

		metadados->localizacao = offset;

		// Salta para a área do diretório.
		seekDiretorio(vpp);

		// Insere na lista todos os metadados do diretório para obter o seu tamanho, que é a posição do novo arquivo.
		lista_insere_metadados(vpp, lista);

		metadados->posicao = (lista_tamanho(lista) + 1);

		// Offset que será atualizado futuramente.
		offset += size;

	} else {
		if(!(vpp = fopen(vppName, "w+"))) {
			fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", vppName);
			exit(1);
		}
		// Primeiro arquivo, posição 1.
		metadados->posicao = 1;
		metadados->localizacao = 4;

		// Escreve a posição que começa o diretório
		offset = (size + sizeof(int));
	}
	// Atualiza o offset
	atualizaOffset(vpp, offset);

	// Insere o novo metadado na lista
	lista_insere_fim(lista, metadados);

	// Escreve os dados.
	transportaBytes(vpp, file);

	// Atualiza o necessário sobre o diretório.
	atualizaDir(vpp, lista);
	// Imprime a lista.
	lista_imprime(lista);
	lista_destroi(lista);
	fclose(vpp);
	fclose(file);
}

void lista_arquivos(char *vppName, char **fileNames) {
	FILE *vpp;
	lista_t *l;
	metadado_t *metadados;

	// Cria a lista.
	if((l = lista_cria()) == NULL) {
		fprintf(stderr, "Erro ao iniciar a lista.\n");
		exit(1);
	}
	// Verifica-se primeiro se o destino está acessível
	// caso esteja, o abre-se o arquivo no modo r+, economizando leitura dos
	// arquivos já inseridos.
	if(access(vppName, F_OK) == 0) {
		if(!(vpp = fopen(vppName, "r"))) {
			fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", vppName);
			exit(1);
		}
	} else {
		fprintf(stderr, "Erro ao abrir o archiver \"%s\".\n", vppName);
		exit(1);
	}

	lista_insere_metadados(vpp, l);

	// Se não há fileNames especificados, imprime todo o conteúdo do archiver.
	if(!(*fileNames)) {
		lista_imprime(l);
		lista_destroi(l);
		fclose(vpp);
	}

	// Se fileNames não é nula, então temos que verificar quais arquivos devem ser listados.
	while(*fileNames) {
		if(!(metadados = getMetadados(l, fileNames[0]))) {
			fprintf(stderr, "Erro: Arquivo inexistente.\n");
			lista_destroi(l);
			fclose(vpp);
			exit(1);
		} else {
			metadado_imprime(metadados);
		}
		fileNames++;
	}
	lista_destroi(l);
	fclose(vpp);
}