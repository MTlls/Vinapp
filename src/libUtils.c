#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libUtils.h"

/**
 * Função que formata os uma string como um caminho relativo.
 */
void formataCaminho(char *dest, char *src) {
	// Independente de ser um caminho relativo ou não, é necessário com que o metadado de localização seja relativo ao diretório do archive. Será inserido um "./" no começo da string de localização.
	dest[0] = '.';

	// Verifica se começa com '/' ou não
	if(src[0] != '/')
		dest[1] = '/';

	// Necessário limitar a string para o strcat.
	dest[2] = '\0';
	// concatena ./ + str
	strcat(dest, src);
}

/**
 * Função que apenas pega a ultima string após o '/'.
 */
void formataNome(char *dest, char *src) {
	int i;
	// Aponta para o primeiro char depois de '/'
	char *ponteiroNome;
	for(i = strlen(src); i >= 0 && src[i] != '/'; i--)
		ponteiroNome = src + i;

	strcpy(dest, ponteiroNome);
	// Autoexplicativo.
	retiraEspacos(dest);
}

/**
 * Função que retira os espaços da sting.
 */
void retiraEspacos(char *str) {
	int aux = 0, tam = strlen(str), i;
	for(i = 0; i < tam; i++) {
		if(str[i] != ' ')
			str[aux++] = str[i];
	}
	str[aux] = '\0';
}

/**
 * Função útil para printar o help, separa em colunas o print.
 */
void imprime_coluna(int recuo, int tamanho_coluna, char *texto) {
	int i;

	for(i = 1; i < strlen(texto) + 1; i++) {
		/* Ajusta o tamanho da coluna */
		if(i % tamanho_coluna == 0) {
			// Se não encontrar um espaço, escreve a palavra até encontrar uma.
			while(texto[i - 1] != ' ') {
				putchar(texto[(i++) - 1]);
			}
			// Quebra a linha e empurra o cursor da saída para escrever logo abaixo
			putchar('\n');
			printf("%*c", recuo-1, ' ');
		}
		putchar(texto[i - 1]);
	}
	putchar('\n');
	putchar('\n');
}