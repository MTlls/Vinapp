#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "opcoes.h"

void extrai(char *vpp, char *nomeArquivo) {
	// FILE *dest, *origem;
	// int posDir, tamArq, localizacao;
	// char bufferChar;
	// char bufferNome[50] = {'\0'};

	// if(!(dest = fopen(nomeArquivo, "w"))) {
	// 	fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", nomeArquivo);
	// 	exit(1);
	// }

	// if(!(origem = fopen(vpp, "r"))) {
	// 	fprintf(stderr, "Erro ao abrir o arquivo \"%s\".\n", vpp);
	// 	exit(1);
	// }

	// fread(&posDir, sizeof(int), 1, origem);
	// fseek(origem, posDir, SEEK_SET);

	// // Verifica a cada laço se estamos no arquivo correto
	// while(strcmp(bufferNome, nomeArquivo) != 0) {
	// 	// Zera o buffer a cada loop
	// 	bufferNome[0] = '\0';

	// 	// Primeira iteração
	// 	fread(&bufferChar, 1, 1, origem);
	// 	strncat(bufferNome, &bufferChar, 1);

	// 	// Concatena os caracteres até terminar o nome do arquivo.
	// 	while(bufferChar != '\0') {
	// 		fread(&bufferChar, 1, 1, origem);
	// 		strncat(bufferNome, &bufferChar, 1);
	// 	}

	// 	// Pula os proximos 2 inteiros
	// 	fseek(origem, sizeof(int) * 2, SEEK_CUR);
	// }

	// // Volta os dois inteiros pulados do cursor
	// fseek(origem, -(sizeof(int) * 2), SEEK_CUR);

	// // Buffer recebe o tamanho
	// fread(&tamArq, sizeof(int), 1, origem);
	// fread(&localizacao, sizeof(int), 1, origem);
	// printf("posição do diretorio: %d\n", posDir);

	// printf("tamanho do arquivo: %d\n", tamArq);
	// // Buffer recebe a posição
	// printf("localização do arquivo: %d\n", localizacao);
	// fseek(origem, localizacao, SEEK_SET);

	// transportadorBytes(dest, origem, tamArq);

	// fclose(dest);
	// fclose(origem);
}

int main(int argc, char **argv) {
	// char *destinoNome;
	char *vpp;
	int i, option;

	// Testa se não há mais opções/argumentos
	if(argc == 1)
		printf("Necessário mais argumentos!\n");

	while((option = getopt(argc, argv, "i:x:r:c:")) != -1) {
		switch(option) {
		case 'i':
			vpp = optarg;

			// Captura o nome dos arquivos e para cada um chama a função
			for(i = optind; i < argc; i++) {
				forja(vpp, argv[optind]);
			}

			break;
		case 'x':
			vpp = optarg;

			// Captura o nome dos arquivos e para cada um chama a função
			for(i = optind; i < argc; i++)
				extrai(vpp, argv[i]);

			break;
		case 'r':
			vpp = optarg;

			// Captura o nome dos arquivos e para cada um chama a função
			for(i = optind; i < argc; i++) {
				remove_arquivo(vpp, argv[i]);
			}
			break;
		case 'c':
			vpp = optarg;
			char **arquivos;
			if(argc > 3) {
				arquivos = argv + 3;
				lista_arquivos(vpp, arquivos);
			} else {
				lista_arquivos(vpp, NULL);
			}
			break;
		case '?':
			printf("Opção inválida!\n");
			break;
		default:
			printf("Opção inválida!\n");
			break;
		}
	}
}