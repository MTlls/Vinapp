#include "libLista.h"

#include <string.h>

#include "libMetadado.h"
#include "saltos.h"

/*
 * Cria uma lista vazia e a retorna, se falhar retorna NULL.
 */

lista_t *lista_cria() {
	lista_t *l;

	/*Testando o malloc*/
	if(!(l = (lista_t *)malloc(sizeof(lista_t))))
		return NULL;

	l->tamanho = 0;
	l->ini = NULL;

	return l;
}

/*
 * Remove todos os elementos da lista, libera espaco e retorna NULL.
 */
lista_t *lista_destroi(lista_t *l) {
	metadado_t elem;

	/* A função retirar do início da free em todos os nodos,
	 * assim, só precisamos
	 * executá-la iterativamente */
	while(lista_vazia(l) == 0)
		lista_retira_inicio(l, &elem);

	free(l);
	return NULL;
}

/*
 * Retorna 1 se a lista esta vazia e 0 caso contrario.
 */
int lista_vazia(lista_t *l) {
	if(lista_tamanho(l) != 0)
		return 0;

	return 1;
}

/*
 * Retorna o tamanho da lista, isto eh, o numero de elementos presentes nela.
 */
int lista_tamanho(lista_t *l) {
	return l->tamanho;
}

/*
 * Insere o elemento no inicio da lista.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
 */
int lista_insere_inicio(lista_t *l, metadado_t *elemento) {
	nodo_l_t *NewNodo;

	/*Testando o malloc*/
	if(!(NewNodo = (nodo_l_t *)malloc(sizeof(nodo_l_t))))
		return 0;

	NewNodo->elemento = elemento;

	/* Movendo o início para frente */
	NewNodo->prox = l->ini;
	l->ini = NewNodo;
	l->tamanho++;

	return 1;
}

/*
 * Insere o elemento no final da lista.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
 */
int lista_insere_fim(lista_t *l, metadado_t *elemento) {
	nodo_l_t *NodoAtual;
	nodo_l_t *NewNode;
	/*Testando o malloc*/
	if(!(NewNode = (nodo_l_t *)malloc(sizeof(nodo_l_t))))
		return 0;

	NewNode->elemento = elemento;
	NewNode->prox = NULL;

	if(lista_vazia(l) == 1)
		l->ini = NewNode;
	else {
		NodoAtual = l->ini;

		// Quando NodoAtual->prox for NULL,
		// estaremos no último elemento da lista.
		while(NodoAtual->prox != NULL)
			NodoAtual = NodoAtual->prox;

		NodoAtual->prox = NewNode;
	}

	l->tamanho++;
	return 1;
}

/* Ordena a lista e retorna a mesma */
lista_t *ordena_lista(lista_t *l) {
	nodo_l_t *NodoAtual;
	nodo_l_t *Aux = NULL;
	metadado_t *temp;

	NodoAtual = l->ini;

	while(NodoAtual != NULL) {
		Aux = NodoAtual->prox;

		while(Aux != NULL) {
			if(NodoAtual->elemento->posicao > Aux->elemento->posicao) {
				/* Trocando os dois nodos */
				temp = NodoAtual->elemento;
				NodoAtual->elemento = Aux->elemento;
				Aux->elemento = temp;
			}
			Aux = Aux->prox;
		}
		NodoAtual = NodoAtual->prox;
	}

	return l;
}

/*
 * Remove o elemento do inicio da lista e o retorna em 'elemento'.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
 */

int lista_retira_inicio(lista_t *l, metadado_t *elemento) {
	nodo_l_t *temp;
	temp = l->ini;

	if(lista_vazia(l))
		return 0;

	*elemento = *(l->ini->elemento);

	/* Passando o início para o nodo da frente */
	l->ini = (l->ini)->prox;
	l->tamanho--;

	temp->prox = NULL;
	desaloca_metadado(temp->elemento);

	free(temp);

	return 1;
}

/**
 * Remove o elemento do final da lista e o retorna em 'elemento'.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
 */
int lista_retira_fim(lista_t *l) {
	nodo_l_t *NodoAtual;

	if(lista_vazia(l))
		return 0;

	/* Se só tem um elemento na lista, removendo-o */
	if((l->ini)->prox == NULL) {
		l->tamanho--;
		free(l->ini);
		return 1;
	}

	/* Indo para o penúltimo elemento da lista */
	NodoAtual = l->ini;
	while((NodoAtual->prox)->prox != NULL)
		NodoAtual = NodoAtual->prox;

	/*NodoAtual aponta para o penúltimo,
	 * logo podemos remover NodoAtual->prox */
	desaloca_metadado(NodoAtual->prox->elemento);
	free(NodoAtual->prox);
	NodoAtual->prox = NULL;
	l->tamanho--;

	return 1;
}

int lista_pertence(lista_t *l, char *nome) {
	nodo_l_t *NodoAtual;

	if(lista_vazia(l))
		return 0;

	NodoAtual = l->ini;

	while(NodoAtual->prox != NULL) {
		/* Se achar o elemento, sai do laço e retorna 1 */
		if(strcmp(NodoAtual->elemento->nome, nome) == 0 ||
		   strcmp(NodoAtual->elemento->caminho, nome) == 0) {
			return 1;
		}
		NodoAtual = NodoAtual->prox;
	}

	if(strcmp(NodoAtual->elemento->nome, nome) == 0 ||
	   strcmp(NodoAtual->elemento->caminho, nome) == 0) {
		return 1;
	}

	/* Se chegou até aqui, é porque não achou */
	return 0;
}

/*
 * Imprime a lista, do inicio ate o fim, este por ultimo, sem espaco no final.
 * Quando a lista estiver vazia, nao imprime nada, nem um \n.
 */
void lista_imprime(lista_t *l) {
	nodo_l_t *NodoAtual;
	NodoAtual = l->ini;

	if(lista_vazia(l) == 0) {
		while(NodoAtual->prox != NULL) {
			metadado_imprime(NodoAtual->elemento);
			NodoAtual = NodoAtual->prox;
		}

		// Imprimindo o último elemento da lista.
		metadado_imprime(NodoAtual->elemento);
		printf("\n");
	}
}

/*
 * Remove o elemento 'elemento' caso ele exista na lista.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
 * Se o elemento nao for encontrado na lista tambem retorna 0.
 */
int lista_retira_elemento(lista_t *l, metadado_t *elemento) {
	nodo_l_t *NodoAtual;
	NodoAtual = l->ini;

	/* Se o elemento não pertence a lista, retorna 0 */
	if(lista_pertence(l, elemento->nome) == 0)
		return 0;
	else {
		if(strcmp(NodoAtual->elemento->nome, elemento->nome) == 0)
			lista_retira_inicio(l, elemento);
		else {
			while(strcmp(NodoAtual->prox->elemento->nome, elemento->nome) != 0)
				NodoAtual = NodoAtual->prox;

			/* NodoAtual é o anterior do Nodo que queremos remover
			 * Logo, podemos trabalhar com NodoAtual->prox */
			nodo_l_t *del;

			/* del é o Nodo a ser deletado */
			del = NodoAtual->prox;
			NodoAtual->prox = (NodoAtual->prox)->prox;

			desaloca_metadado(del->elemento);

			del->prox = NULL;
			l->tamanho--;
			free(del);
		}
	}
	return 1;
}

/*** FUNÇÕES NOVAS ******/

/**
 * Função que troca os ponteiros da lista e os reordena.
 */
void lista_troca(lista_t *l, nodo_l_t *nodo_1, nodo_l_t *nodo_2) {
	nodo_l_t *anterior1 = l->ini;
	nodo_l_t *aux1, *aux2;

	if(nodo_1->elemento->posicao > 1) {
		// Acha os nodos anteriores, caso ele não seja o primeiro
		while(anterior1->prox != nodo_1)
			anterior1 = anterior1->prox;
		aux2 = nodo_1->prox;
		anterior1->prox = aux2;
	}

	/* Trocando os dois nodos */
	aux1 = nodo_2->prox;
	nodo_2->prox = nodo_1;
	nodo_1->prox = aux1;
}

/**
 * Função que insere todos os metadados do diretório na lista.
 */
void lista_insere_metadados(FILE *file, lista_t *l) {
	metadado_t *buffer;
	seekDiretorio(file);

	if((buffer = aloca_metadado()) == NULL) {
		fprintf(stderr, "Erro ao alocar metadado.\n");
		exit(1);
	}

	// Captura o metadado.
	while(le_metadado(file, buffer) == 1) {
		lista_insere_fim(l, buffer);
		if((buffer = aloca_metadado()) == NULL) {
			fprintf(stderr, "Erro ao alocar metadado.\n");
			exit(1);
		}
	}
	desaloca_metadado(buffer);
}

/*
 * Insere a lista no arquivo do inicio ate o fim.
 */
void lista_escreve_arquivo(FILE *file, lista_t *l) {
	nodo_l_t *NodoAtual;
	NodoAtual = l->ini;

	seekDiretorio(file);

	if(lista_vazia(l) == 0) {
		while(NodoAtual->prox != NULL) {
			escreve_metadado(file, NodoAtual->elemento);
			NodoAtual = NodoAtual->prox;
		}

		// Imprimindo o último elemento da lista.
		escreve_metadado(file, NodoAtual->elemento);
	}
}

/**
 * Função que retrocede os dados necessários dos metadados posteriores a ele.
 * Dados necessários = posicao e localizacao.
 * */
void lista_altera_dados(nodo_l_t *inicio, nodo_l_t *ultimo, altera_lista_t modo, int tamanho) {
	nodo_l_t *NodoAtual;
	NodoAtual = inicio->prox;

	switch(modo) {
	case DIMINUI_RETROCEDE:
		// Retrocede-se todas as posições e as localizacoes.
		while(NodoAtual != ultimo->prox) {
			NodoAtual->elemento->posicao -= 1;
			NodoAtual->elemento->localizacao -= tamanho;
			NodoAtual = NodoAtual->prox;
		}
		break;
	case AUMENTA_AVANCA:
		// Avança-se todas as posicoes e as localizacoes
		while(NodoAtual != ultimo->prox) {
			NodoAtual->elemento->posicao += 1;
			NodoAtual->elemento->localizacao += tamanho;
			NodoAtual = NodoAtual->prox;
		}
		break;
	case AUMENTA:
		// Avança todas as localizacoes.
		while(NodoAtual != ultimo->prox) {
			NodoAtual->elemento->localizacao += tamanho;
			NodoAtual = NodoAtual->prox;
		}
		break;
	case DIMINUI:
		// Retrocede todas as localizacoes.
		while(NodoAtual != ultimo->prox) {
			NodoAtual->elemento->localizacao -= tamanho;
			NodoAtual = NodoAtual->prox;
		}
		break;
	}
}

/**
 * Função que retorna o nodo da lista que possui o metadado com o nome especificado. Retorna NULL caso contrário.
 */
nodo_l_t *getNodo(lista_t *l, char *nome) {
	nodo_l_t *NodoAtual;
	NodoAtual = l->ini;

	while(NodoAtual->prox != NULL) {
		if(strcmp(NodoAtual->elemento->nome, nome) == 0 ||
		   strcmp(NodoAtual->elemento->caminho, nome) == 0) {
			return NodoAtual;
		}
		NodoAtual = NodoAtual->prox;
	}
	// Último nodo
	if(strcmp(NodoAtual->elemento->nome, nome) == 0 ||
	   strcmp(NodoAtual->elemento->caminho, nome) == 0) {
		return NodoAtual;
	}
	return NULL;
}

/**
 * Função que retorna o metadado com o nome especificado. Retorna NULL caso contrário.
 */
metadado_t *getMetadados(lista_t *l, char *nome) {
	nodo_l_t *nodo;
	nodo = l->ini;

	if(!(nodo = getNodo(l, nome))) {
		return NULL;
	}

	return nodo->elemento;
}