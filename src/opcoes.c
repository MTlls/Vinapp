#include "opcoes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "data.h"
#include "libLista.h"
#include "libUtils.h"
#include "opcoes.h"
#include "saltos.h"

#define RECUO 45
#define TAMANHO_COLUNA 60
#define TEXTO_INSERE "Insere/acrescenta um ou mais membros ao archive. Caso o membro já exista no archive, ele deve ser substituído. Novos membros são inseridos respeitando a ordem da linha de comando, ao final do archive."
#define TEXTO_REMOVE "Remove os membros indicados de archive."
#define TEXTO_MOVE "Move o membro indicado na linha de comando para imediatamente depois do membro target existente em archive. A movimentação deve ocorrer na seção de dados do archive"
#define TEXTO_ATUALIZA "Mesmo comportamento da opção -i, mas a substituição de um membro existente ocorre APENAS caso o parâmetro seja mais recente que o arquivado."
#define TEXTO_EXTRAI "Extrai os membros indicados de archive. Se os membros não forem indicados, todos devem ser extraídos. A extração consiste em ler o membro de archive e criar um arquivo correspondente, com conteúdo idêntico, em disco."
#define TEXTO_LISTA "Lista o conteúdo de archive em ordem, incluindo as propriedades de cada membro."
#define TEXTO_AJUDA "Imprime essa pequena mensagem de ajuda com as opções disponíveis e encerra."

void remove_arquivo(char *vpp_name, char *fileName) {
	// vpp_adiantado é o vpp, só que a frente do ponto de leitura do vpp.
	// Será usado para prevenir fseeks seguidos.
	FILE *vpp, *vpp_adiantado;
	lista_t *lista = NULL;

	// Para ficar mais legível.
	nodo_l_t *nodo_alvo = NULL;
	metadado_t *metadado_alvo = NULL;
	int inicio, fim, file_size, l_tamanho;

	if(!(vpp = fopen(vpp_name, "r+"))) {
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
		truncate(vpp_name, sizeof(int));
		lista_destroi(lista);
		fclose(vpp);
		return;
	}

	// Localizacao e final do arquivo alvo
	inicio = metadado_alvo->localizacao;
	file_size = metadado_alvo->tamanho;
	fim = metadado_alvo->localizacao + metadado_alvo->tamanho;

	// Retrocede a posicao de todos os metadados posteriores.
	lista_altera_dados(nodo_alvo, NULL, DIMINUI_RETROCEDE, metadado_alvo->tamanho);
	// Retira o metadado alvo.
	lista_retira_elemento(lista, metadado_alvo);
	// Escreve a lista no arquivo.
	lista_escreve_arquivo(vpp, lista);

	// Exclui o último metadado que está sobrando.
	truncate(vpp_name, ftell(vpp));

	// Será aberto mais outro FILE* para a leitura simultânea do mesmo arquivo.
	// Só será necessário ler essa stream, por isso o modo "r".
	if(!(vpp_adiantado = fopen(vpp_name, "r"))) {
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
	transporta_bytes(vpp, vpp_adiantado);

	// Exclui o lixo que estava após o shift de dados.
	truncate(vpp_name, ftell(vpp));
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
void insercao(char *vpp_name, char *fileName, tipo_insercao_t modo) {
	FILE *vpp, *file, *vpp_ajudante;
	lista_t *lista;
	metadado_t *metadados;
	int offset = 0, size = 0;
	struct stat info;
	char caminho_format[4096];
	nodo_l_t *antigo;

	if(stat(fileName, &info)) {
		fprintf(stderr, "Erro ao obter o status do arquivo \"%s\".\n", fileName);
		exit(1);
	}
	size = info.st_size;

	if(!(file = fopen(fileName, "r"))) {
		fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", fileName);
		exit(1);
	}
	// Captura os metadados do arquivo.
	if((metadados = getStats(info, fileName)) == NULL) {
		fprintf(stderr, "Erro ao gerar os metadados do arquivo \"%s\".\n", fileName);
		fclose(file);
		exit(1);
	}

	// Cria a lista.
	if((lista = lista_cria()) == NULL) {
		fprintf(stderr, "Erro ao iniciar a lista.\n");
		exit(1);
	}

	// Verifica-se primeiro se o destino está acessível
	// caso esteja, o abre-se o arquivo no modo r+, economizando leitura dos
	// arquivos já inseridos.
	if(access(vpp_name, F_OK) == 0) {
		if(!(vpp = fopen(vpp_name, "r+"))) {
			fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", vpp_name);
			exit(1);
		}

		// Captura a posição que começa o diretório
		fread(&offset, sizeof(int), 1, vpp);

		// Insere na lista todos os metadados do diretório para obter o seu tamanho, que é a posição do novo arquivo.
		lista_insere_metadados(vpp, lista);

		// Se a lista possui nodos, já é necessário verificar se ele já está dentro do archiver.
		if(lista_tamanho(lista) > 0) {
			// Formata-se o caminho para verificar se ele já existe no archiver.
			formataCaminho(caminho_format, fileName);
			if(lista_pertence(lista, caminho_format)) {
				// Para alterar as propriedades da lista e o próprio nodo.
				antigo = getNodo(lista, caminho_format);
				// Opcao -a, verificacao se o arquivo é mais atual ou não.
				if(modo == OPTION_A) {
					// Se não pode atualizar, apresenta erro e sai.
					if(!podeAtualizar(antigo->elemento, metadados)) {
						fprintf(stderr,
						        "Erro: Não foi possível atualizar/inserir o arquivo. \"%s\" não é mais recente que o \"%s\" dentro do archiver.\n",
						        fileName, antigo->elemento->caminho);
						exit(1);
					}
				}

				if(!(vpp_ajudante = fopen(vpp_name, "r+"))) {
					fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", vpp_name);
					exit(1);
				}
				// Substituicao do arquivo
				substitui(vpp, vpp_ajudante, file, antigo, metadados);
				// Truncado no começo da area diretório.
				truncate(vpp_name, getOffset(vpp));
				// Atualizado o diretorio.
				atualizaDir(vpp, lista);

				lista_destroi(lista);
				desaloca_metadado(metadados);
				fclose(vpp_ajudante);
				fclose(vpp);
				fclose(file);
				return;
			}
		}

		metadados->localizacao = offset;

		metadados->posicao = (lista_tamanho(lista) + 1);

		// Offset que será atualizado futuramente.
		offset += size;

	} else {
		if(!(vpp = fopen(vpp_name, "w+"))) {
			fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", vpp_name);
			exit(1);
		}
		// Primeiro arquivo, posição 1.
		metadados->posicao = 1;
		metadados->localizacao = 4;

		// Atualiza o offset
		atualizaOffset(vpp, sizeof(int));

		// Escreve a posição que começa o diretório
		offset = (size + sizeof(int));
	}
	seekDiretorio(vpp);

	// Escreve os dados.
	transporta_bytes(vpp, file);

	// Atualiza o offset
	atualizaOffset(vpp, offset);

	// Insere o novo metadado na lista
	lista_insere_fim(lista, metadados);

	// Atualiza o necessário sobre o diretório.
	atualizaDir(vpp, lista);
	// Imprime a lista.
	lista_imprime(lista);
	lista_destroi(lista);
	fclose(vpp);
	fclose(file);
}

void lista_arquivos(char *vpp_name, char **fileNames) {
	FILE *vpp;
	lista_t *l;
	metadado_t *metadados;

	// Verifica-se primeiro se o destino está acessívels.
	if(access(vpp_name, F_OK) == 0) {
		if(!(vpp = fopen(vpp_name, "r"))) {
			fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", vpp_name);
			exit(1);
		}
	} else {
		fprintf(stderr, "Erro ao abrir o archiver \"%s\".\n", vpp_name);
		exit(1);
	}
	// Cria a lista.
	if((l = lista_cria()) == NULL) {
		fprintf(stderr, "Erro ao iniciar a lista.\n");
		exit(1);
	}

	lista_insere_metadados(vpp, l);

	// Se não há fileNames especificados, imprime todo o conteúdo do archiver.
	if(!(fileNames)) {
		lista_imprime(l);
		lista_destroi(l);
		fclose(vpp);
		return;
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

/**
 * Imprime o manual de ajuda.
 */
void help() {
	printf("%-*s", RECUO, "-i [ARCHIVER] [ARQUIVO1, ARQUIVO2, ...]");
	imprime_coluna(RECUO, TAMANHO_COLUNA, TEXTO_INSERE);

	printf("%-*s", RECUO, "-r [ARCHIVER] [ARQUIVO1, ARQUIVO2, ...]");
	imprime_coluna(RECUO, TAMANHO_COLUNA, TEXTO_REMOVE);

	printf("%-*s", RECUO, "-a [ARCHIVER] [ARQUIVO1, ARQUIVO2, ...]");
	imprime_coluna(RECUO, TAMANHO_COLUNA, TEXTO_ATUALIZA);

	printf("%-*s", RECUO, "-x [ARCHIVER] [ARQUIVO1, ARQUIVO2, ...]");
	imprime_coluna(RECUO, TAMANHO_COLUNA, TEXTO_EXTRAI);

	printf("%-*s", RECUO, "-m [ARCHIVER] [ARQUIVO1, ARQUIVO2]");
	imprime_coluna(RECUO, TAMANHO_COLUNA, TEXTO_MOVE);

	printf("%-*s", RECUO, "-c [ARCHIVER] [ARQUIVO1, ARQUIVO2, ...]");
	imprime_coluna(RECUO, TAMANHO_COLUNA, TEXTO_LISTA);

	printf("%-*s", RECUO, "-h");
	imprime_coluna(RECUO, TAMANHO_COLUNA, TEXTO_AJUDA);
}

/**
 * Função que substitui o arquivo antigo pelo novo de mesmo nome.
 */
void substitui(FILE *vpp, FILE *auxiliar, FILE *file, nodo_l_t *antigo, metadado_t *novo) {
	int offset = getOffset(vpp),
	    new_tam = novo->tamanho,
	    old_tam = antigo->elemento->tamanho,
	    old_loc = antigo->elemento->localizacao,
	    new_final = old_loc + new_tam - 1,
	    old_final = old_loc + old_tam - 1,
	    prox_loc = old_final + 1;

	int diferenca, bloco;

	// Caso o tamanho do novo arquivo seja maior, é só inverter o sinal.
	diferenca = (new_tam - old_tam);

	// Caso seja inserido arquivo maior que o antigo.
	if(new_tam > old_tam) {
		printf("Arquivo antigo é menor que o novo!\n");

		// Será shiftado todos os arquivos entre o diretorio e o arquivo substituido.
		bloco = offset - prox_loc;

		// Ponto de escrita.
		fseek(auxiliar, offset + diferenca - 1, SEEK_SET);
		// Ponto de leitura.
		fseek(vpp, offset - 1, SEEK_SET);

		// Realizado o shift
		shift_direita(auxiliar, vpp, bloco);

		// Substitui o tamanho, uid, data de modificao e permissoes.
		substitui_metadados(antigo->elemento, novo);
		lista_altera_dados(antigo, NULL, AUMENTA, diferenca);
	}
	// Caso contrário.
	else if(new_tam < old_tam) {
		printf("Arquivo novo menor que o antigo.\n");
		diferenca = -diferenca;

		// Final dos arquivos é os finais + 1.
		fseek(auxiliar, new_final + 1, SEEK_SET);
		fseek(vpp, old_final + 1, SEEK_SET);
		transporta_bytes(vpp, vpp);

		// Substitui o tamanho, uid, data de modificao e permissoes.
		substitui_metadados(antigo->elemento, novo);
		lista_altera_dados(antigo, NULL, DIMINUI, diferenca);

		// Soh para a diferença voltar ao negativo :)
		diferenca = -diferenca;
	} else {
		printf("Arquivo tem o mesmo tamanho que o novo.\n");
		fseek(vpp, old_loc, SEEK_SET);
	}

	// Escreve o arquivo
	fseek(vpp, old_loc, SEEK_SET);
	transporta_bytes(vpp, file);
	// Atualiza o novo offset, vai depender da diferença entre os dois arquivos.
	atualizaOffset(vpp, (offset + diferenca));
}

void offset(char *vpp_name) {
	FILE *vpp;
	if(!(vpp = fopen(vpp_name, "r"))) {
		fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", vpp_name);
		exit(1);
	}
	printf("offset:%d\n", getOffset(vpp));
}

void movimenta(char *vpp_name, char *target_name, char *member_name) {
	FILE *vpp, *vpp_ajudante;
	lista_t *l;
	metadado_t *target, *member;
	nodo_l_t *member_n, *target_n;

	// Tamanho máximo de um caminho!
	char target_name_format[4096], member_name_format[4096];
	int member_size, member_final,
	    qntDados_shiftado, target_next, target_final, member_next;

	if(!(vpp = fopen(vpp_name, "r+"))) {
		fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", vpp_name);
		exit(1);
	}

	if(!(vpp_ajudante = fopen(vpp_name, "r+"))) {
		fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", vpp_name);
		exit(1);
	}

	if((l = lista_cria()) == NULL) {
		fprintf(stderr, "Erro ao iniciar a lista.\n");
		fclose(vpp);
		fclose(vpp_ajudante);
		exit(1);
	}

	// Formata os dois caminhos para começar a movimentacao
	formataCaminho(target_name_format, target_name);
	formataCaminho(member_name_format, member_name);

	// Insere os metadados na lista
	lista_insere_metadados(vpp, l);

	if(!(lista_pertence(l, target_name_format)) || !(lista_pertence(l, member_name_format))) {
		fprintf(stderr, "Não existe esses arquivos no archiver, tente com algum desses:\n");
		lista_imprime(l);
		exit(1);
	}

	// Captura os respectivos metadados
	target_n = getNodo(l, target_name_format);
	member_n = getNodo(l, member_name_format);

	if(target_n->prox == member_n) {
		fprintf(stdout, "Arquivo já é o próximo imediato do target. Segue a lista dos arquivos:\n");
		lista_imprime(l);
		exit(0);
	}

	target = target_n->elemento;
	member = member_n->elemento;

	// Tamanho de cada arquivo
	member_size = member->tamanho;
	// target_size = target->tamanho;
	member_final = (member->localizacao + member->tamanho - 1);
	target_final = (target->localizacao + target->tamanho - 1);
	target_next = target_final + 1;
	member_next = member_final + 1;

	// Vpp aponta para o começo do diretorio e o  ajudante para o começo do member.
	seekDiretorio(vpp);
	fseek(vpp_ajudante, member->localizacao, SEEK_SET);

	// Movimenta member para o final do arquivo para facilitar
	transporta_n_bytes(vpp, vpp_ajudante, member_size);

	// Trunca para não haver dados sobrando.
	truncate(vpp_name, ftell(vpp));

	// Os shifts são diferentes dependendo da posicao do target e do member
	// Primeiro caso: member está a frente de target.
	if(target->posicao < member->posicao) {
		// qntDados_shiftado = final do arquivo anterior ao member - começo arquivo depois de target
		qntDados_shiftado = (member->localizacao - 1) - target_next;

		// vpp aponta para o final do member
		fseek(vpp, member_final, SEEK_SET);

		// ajudante aponta para o final do membro anterior a ele
		fseek(vpp_ajudante, member->localizacao - 1, SEEK_SET);

		// Shift a direita de tudo entre target e member
		shift_direita(vpp, vpp_ajudante, qntDados_shiftado);

		// vpp aponta para o proximo arquivo de target
		fseek(vpp, target_next, SEEK_SET);

		// ajudante aponta para a area do diretorio contendo os dados de member
		seekDiretorio(vpp_ajudante);

		// Transporta o member para depois de target.
		transporta_bytes(vpp, vpp_ajudante);

		// Altera todos os dados posteriores de file1
		lista_altera_dados(target_n, member_n, AUMENTA_AVANCA, member->tamanho);
		
		// Troca as posições dos arquivos (nos metadados).
		member->posicao = target->posicao + 1;
		member->localizacao =  target_next;

		// Troca os ponteiros de prox na lista
		lista_troca(l, member_n, target_n);

		// Atualiza o diretório.
		atualizaDir(vpp, l);
	}
	// Segundo caso: member está antes de target
	else {
		// qntDados_shiftado = final do arquivo target - começo arquivo depois de member
		qntDados_shiftado = (target_final) - (member_next);

		// vpp aponta para o começo de member
		fseek(vpp, member_final, SEEK_SET);

		// ajudante aponta para comeco do membro posterior a ele
		fseek(vpp_ajudante, member_next, SEEK_SET);

		// Shift a direita de tudo entre target e member
		transporta_n_bytes(vpp, vpp_ajudante, qntDados_shiftado);

		// vpp aponta para o proximo arquivo de target
		fseek(vpp, target_next - member_size, SEEK_SET);

		// ajudante aponta para a area do diretorio contendo os dados de member
		seekDiretorio(vpp_ajudante);

		// Transporta o member para depois de target.
		transporta_bytes(vpp, vpp_ajudante);

		// Altera todas as posicoes
		lista_altera_dados(member_n, target_n, DIMINUI_RETROCEDE, member->tamanho);
		
		// Troca as posições e localizacoes dos arquivos (nos metadados).
		member->posicao = target->posicao +1;
		member->localizacao = target->localizacao + target->tamanho;

		// Troca o proximo ponteiro de target pelo membro
		lista_troca(l, member_n, target_n);

		// Atualiza o diretório.
		atualizaDir(vpp, l);
	}
	// Trunca para não haver dados sobrando.
	truncate(vpp_name, ftell(vpp));
}
