#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "opcoes.h"

int main(int argc, char **argv) {
	int i, option;

	// Testa se não há mais opções/argumentos
	if(argc == 1)
		printf("Necessário mais argumentos!\n");

	while((option = getopt(argc, argv, "hi:x:r:c:a:m:")) != -1) {
		switch(option) {
		case 'i':
			// Captura o nome dos arquivos e para cada um chama a função
			for(i = optind; i < argc; i++) {
				insercao(optarg, argv[i], OPTION_I);
			}

			break;
		case 'x':
			// Captura o nome dos arquivos e para cada um chama a função
			for(i = optind; i < argc; i++)
				// extrai(optarg, argv[i]);

				break;
		case 'r':
			// Captura o nome dos arquivos e para cada um chama a função
			for(i = optind; i < argc; i++) {
				remove_arquivo(optarg, argv[i]);
			}
			break;
		case 'c':
			char **arquivos;
			if(argc > 3) {
				arquivos = argv + 3;
				lista_arquivos(optarg, arquivos);
			} else {
				lista_arquivos(optarg, NULL);
			}
			break;

		case 'h':
			// Lista o help.
			help();
			break;
		case 'a':
			// Captura o nome dos arquivos e para cada um chama a função
			for(i = optind; i < argc; i++) {
				insercao(optarg, argv[i], OPTION_A);
			}
			break;
		case 'm':
			// Captura o nome dos arquivos e para cada um chama a função
			movimenta(argv[optind], optarg, argv[optind + 1]);
			break;
		case '?':
			printf("Opção inválida! Digite \"./vina++ -h\" para ver as opções.\n");
			help();
			break;
		default:
			printf("Opção inválida! Digite \"./vina++ -h\" para ver as opções.\n");
			break;
		}
	}
}