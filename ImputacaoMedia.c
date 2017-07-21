// TERMINARRRR

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>


typedef struct base{
	char nome[50];
	int qtd_atributos;
	int qtd_rotulos;
	int qtd_atributos_rotulos;
	int porcentagem;
	int qtd_atributos_tirar;
	double * medias;
	int * qtd_elementos_medias;
} Base;


Base * alocarBase (){
	Base * base = (Base *) malloc (sizeof (Base));
	if (base != NULL){
		base->qtd_atributos = 0;
		base->qtd_rotulos = 0;
		base->qtd_atributos_rotulos = 0;
		base->qtd_atributos_tirar = 0;
		return base;
	}
}

// Aloca vetor de inteiros e iniciliza com -1
int * aloca_vetor (int tamanho){
	int * vetor = (int *) malloc (sizeof (int) * tamanho);
	if (vetor != NULL){
		int i;
		for (i = 0; i < tamanho; i++){
			vetor[i] = -1;
		}
		return vetor;
	}
}

// Aloca vetor de doubles e iniciliza com -1
double * aloca_vetor_double (int tamanho){
	double * vetor = (double *) malloc (sizeof (double) * tamanho);
	if (vetor != NULL){
		int i;
		for (i = 0; i < tamanho; i++){
			vetor[i] = -1;
		}
		return vetor;
	}
}


// Inicializa vetor de doubles
void inicializa_vetor_double (double * vetor, int tamanho){
	if (vetor != NULL){
		int i;
		for (i = 0; i < tamanho; i++){
			vetor[i] = -1;
		}
	}	
}


// Inicializa vetor de inteiros
void inicializa_vetor (int * vetor, int tamanho){
	if (vetor != NULL){
		int i;
		for (i = 0; i < tamanho; i++){
			vetor[i] = -1;
		}
	}	
}

// Inicializa vetor de doubles
void zera_vetor_double (double * vetor, int tamanho){
	if (vetor != NULL){
		int i;
		for (i = 0; i < tamanho; i++){
			vetor[i] = 0;
		}
	}	
}


// Zerar vetor de inteiros
void zera_vetor (int * vetor, int tamanho){
	if (vetor != NULL){
		int i;
		for (i = 0; i < tamanho; i++){
			vetor[i] = 0;
		}
	}	
}


// Imprime vetor de doubles
void imprimir_vetor_double (double * vetor, int tamanho){
	int i;
	for (i = 0; i < tamanho; i++){
		printf ("%d: %g\t", i, vetor[i]);
	}
	printf ("\n");
}


// Imprime vetor de inteiros
void imprimir_vetor (int * vetor, int tamanho){
	int i;
	printf("\n");
	for (i = 0; i < tamanho; i++){
		printf ("%d: %d\n", i, vetor[i]);
	}
}


// Encontra substring a partir de uma string
char * encontrar_substring (char * string, int inicio, int tamanho){
	if (string == NULL || inicio < 0 || tamanho <= 0)
		return NULL;
	char * substring = (char *) malloc (sizeof (char) * (tamanho + 1));
	if (substring == NULL){
		printf("Nao ha memoria suficiente para alocar substring\n");
		return NULL;
	}
	int i;
	for (i = 0; i < tamanho; i++)
		substring[i] = string[inicio + i];
	substring[i] = '\0';
	return substring;
}

// Descobre quantos rótulos existem na base de dados
int descobrir_qtd_rotulos(FILE * arquivo){
	char char_anterior, char_atual, *numero, linha_arquivo[50];
	int i = 1, qtd_caracteres = 0, qtd_rotulos;

	// Lê linhas do arquivo e verifica se não são comentários
	do{
		fgets (linha_arquivo, sizeof (linha_arquivo), arquivo);
		
	}
	while (linha_arquivo[0] == '%');

	// Descobre quantidade de rótulos 
	while (1){
		char_anterior = linha_arquivo[i-1];
		char_atual = linha_arquivo[i];
			if (char_anterior == '-' && char_atual == 'C'){
				i += 2;
				if (linha_arquivo[i] == '-')
					i += 1;
				while (isdigit(linha_arquivo[i])){
					qtd_caracteres++;
					i++;
				}
				numero = encontrar_substring(linha_arquivo, i - qtd_caracteres, qtd_caracteres);
				qtd_rotulos = atoi(numero);
				break;
			}	
		i++;
	}
	return qtd_rotulos;
}

// Descobre quantos atributos existem na base de dados (incluindo qtd de rótulos)
int descobrir_qtd_atributos(FILE * arquivo){
	int qtd_atributos = 0;
	size_t tamanho = 100; //valor arbitrario
	char * linha = malloc(tamanho);
	if (linha != NULL){
		do{
			getline(&linha, &tamanho, arquivo);
			if (strcmp(encontrar_substring(linha, 0, 10), "@attribute") == 0)
				qtd_atributos++; 
		}
		while (strcmp (linha, "@data\n") != 0);
		return qtd_atributos;
	}
	return -1;
}

//Escreve parte inicial do arquivo fornecido no arquivo de saída
void escreve_parte1_arquivo(FILE * entrada, FILE * saida){
	if (entrada != NULL){
		size_t tamanho = 100; //valor arbitrario
		char * linha = malloc(tamanho);
		if (linha != NULL){
			fseek(entrada, 0, SEEK_SET);
			do{
				getline(&linha, &tamanho, entrada);
				fprintf(saida, "%s", linha);
			}
			while (strcmp (linha, "@data\n") != 0);
		}

	}
	
}

// Conta e soma todos os atributos válidos para poder calcular a média
void somar_atributos (double * vetor, Base * base){
	int i, j;	
	for (i = base->qtd_rotulos, j = 0; i < base->qtd_atributos_rotulos; i++, j++){
		if (vetor[i] != -1){
			base->medias[j] += vetor[i];
			base->qtd_elementos_medias[j] += 1;
		}
	}
}

void calcular_media (Base * base, int flag_base_binaria){
	int i;
	if (flag_base_binaria == 1){
		for (i = 0; i < base->qtd_atributos; i++){
			if (base->medias[i] / base->qtd_elementos_medias[i] < 0.6)
				base->medias[i] = 0;
			else
				base->medias[i] = 1;
			//base->medias[i] = base->medias[i] / base->qtd_elementos_medias[i];
			
		}
	}
	else{
		for (i = 0; i < base->qtd_atributos; i++){
			base->medias[i] = base->medias[i] / base->qtd_elementos_medias[i];
		}
	}
}

// Recebe linha do arquivo (instância) e carrega todos os atributos em um vetor 
void carrega_atributos_vetor (char * linha, double * vetor, int qtd_atributos_rotulos){
	if (linha != NULL && vetor != NULL && qtd_atributos_rotulos > 0){
		int i = 0, j = 0, j_inicio = 0;
		char * valor;
		while (linha[j] != '\0'){
			if (linha[j] == ','){
				valor = encontrar_substring (linha, j_inicio, j - j_inicio);
				vetor[i] = atof(valor);
				free(valor);
				valor = NULL;
				i++;
				j++;

				// Verifica se o próximo atributo é ausente
				while (linha[j] == ','){
					valor = "-1";
					vetor[i] = atof(valor);
					i++;
					j++;
				}
				// Marca início de novo atributo existente
				j_inicio = j;
			}
			j++;
		}
		// Verifica se o último atributo é ausente
		if (j - 1 - j_inicio == 0)
		{
			valor = "-1";
			vetor[i] = atof(valor);
		}
		else{
			valor = encontrar_substring (linha, j_inicio, j - 1 - j_inicio);
			vetor[i] = atof(valor);
		}
	}
}

void encontrar_instancias (FILE * arquivo, FILE * saida, Base * base, int flag_base_binaria){
	if (arquivo != NULL || base != NULL){
		int flag_instancias = 0, contador = 0;
		size_t tamanho = 100; //valor arbitrario
		char * linha = malloc(tamanho);
		//char * instancia = NULL;
	
		if (linha != NULL){
			
			// Aloca vetor referente a uma instância do arquivo que será utilizada no cálculo da média
			double * vetor = aloca_vetor_double(base->qtd_atributos_rotulos);
			if (vetor == NULL){
				printf("ocorreu um erro!\n");
				return;
			}
			
			//PENSARRR MELHORRR
			char * instancia = (char*) malloc (sizeof (double) * base->qtd_atributos_rotulos * 2);
			if (instancia == NULL){
				printf("ocorreu um erro!\n");
				return;
			}
			while (getline(&linha, &tamanho, arquivo) > 0){
			//while (getline(&linha, &tamanho, arquivo) > 0 && contador < 6){
				inicializa_vetor_double(vetor, base->qtd_atributos_rotulos);
				carrega_atributos_vetor (linha, vetor, base->qtd_atributos_rotulos);	
				somar_atributos (vetor, base);
				//imprimir_vetor_double(base->medias, base->qtd_atributos);
				contador++;
			}
			calcular_media (base, flag_base_binaria);
			//printf("contador: %d\n", contador);
		}
	}	
}

//Percorre parte inicial do arquivo fornecido no arquivo de saída
void percorre_parte1_arquivo(FILE * entrada, FILE * saida){
	if (entrada != NULL){
		size_t tamanho = 100; //valor arbitrario
		char * linha = malloc(tamanho);
		if (linha != NULL){
			fseek(entrada, 0, SEEK_SET);
			do{
				getline(&linha, &tamanho, entrada);
			}
			while (strcmp (linha, "@data\n") != 0);
		}
	}
}

char* vetor_to_instancia(double * vetor, char * instancia, int qtd_atributos_rotulos){
	if (vetor != NULL && qtd_atributos_rotulos > 0){
		int i;
		char valor_atributo[20];
		// PENSAR NUMA FORMA MELHOR DE ALOCAR ESTE VETOR QUE É DINAMICO (varia de acordo com o tamanho dos numeros)
		// FOI CONSIDERADO O TAMANHO MAXIMO (qtd de atributos doubles e virgulas)
		// PARA EVITAR MALLOC TD HR PODERIA SOMENTE LIMPAR A INSTANCIA, ESTÁ CERTO???
		instancia[0] = '\0';
		
		for (i = 0; i < qtd_atributos_rotulos-1; i++){
			if (vetor[i] == -1)
				sprintf (valor_atributo, ",");
			
			else
				sprintf (valor_atributo, "%g,", vetor[i]);
			strcat(instancia, valor_atributo);
		}
		if (vetor[i] == -1)
			sprintf (valor_atributo, "\n");
		else
			sprintf (valor_atributo, "%g\n", vetor[i]);
		strcat(instancia, valor_atributo);
		return instancia;
	}
}


void imputacao_media (FILE * arquivo, FILE * saida, Base * base){
	if (arquivo != NULL || base != NULL){
		int i, j, contador = 0;
		size_t tamanho = 100; //valor arbitrario
		char * linha = malloc(tamanho);
		//char * instancia = NULL;
	
		if (linha != NULL){
			
			// Aloca vetor referente a uma instância que terá valores imputados
			double * vetor = aloca_vetor_double(base->qtd_atributos_rotulos);
			if (vetor == NULL){
				printf("ocorreu um erro!\n");
				return;
			}
			
			//PENSARRR MELHORRR
			char * instancia = (char*) malloc (sizeof (double) * base->qtd_atributos_rotulos * 2);
			if (instancia == NULL){
				printf("ocorreu um erro!\n");
				return;
			}
			while (getline(&linha, &tamanho, arquivo) > 0){
			//while (getline(&linha, &tamanho, arquivo) > 0 && contador < 6){
				inicializa_vetor_double(vetor, base->qtd_atributos_rotulos);
				carrega_atributos_vetor (linha, vetor, base->qtd_atributos_rotulos);	
				
				for (i = base->qtd_rotulos, j = 0; i < base->qtd_atributos_rotulos; i++, j++){
					if (vetor[i] == -1){
						vetor[i] = base->medias[j];
					}
				}
						
				vetor_to_instancia(vetor, instancia, base->qtd_atributos_rotulos);
				fprintf(saida, "%s", instancia);
				contador++;
			}
			//printf("contador: %d\n", contador);
		}
	}	
}

int main(){
	char nome_arquivo_saida[70];
	
	int flag_base_binaria;
	Base * base = alocarBase();
	if (base == NULL){
		printf ("ocorreu um erro na alocacao da base\n");
	}	
	
	// Usuário informa a base de dados que será alterada
	printf("Digite o nome do arquivo, ou seja, da base de dados (colocar a extensao)):\n");
	scanf ("%s", base->nome);

	// Usuário informa se a base de dados é binária
	printf("Esta base de dados e binaria?\n1 - Sim\n2 - Nao\n");
	scanf ("%d", &flag_base_binaria);

	// Abre arquivo de entrada
	FILE * entrada = fopen(base->nome, "r");
	if (entrada == NULL){
		printf("Arquivo nao encontrado!\n");
		return 0;
	}

	int tam_nome_arquivo = strlen (base->nome);
	sprintf(nome_arquivo_saida, "%.*s_ImputacaoMedia.arff", tam_nome_arquivo-5, base->nome);

	// Abre arquivo de saída
	FILE * saida = fopen(nome_arquivo_saida, "a+");
	if (saida == NULL){
		printf("Arquivo nao encontrado!\n");
		return 0;
	}

	// Descobre quantos rótulos existem na base de dados
	base->qtd_rotulos = descobrir_qtd_rotulos(entrada);
	printf("qtd_rotulos: %d\n", base->qtd_rotulos);

	// Descobre quantos atributos existem na base de dados (incluindo rotulos)
	base->qtd_atributos_rotulos = descobrir_qtd_atributos(entrada);
	if (base->qtd_atributos_rotulos == -1){
		printf("ocorreu um erro!\n");
		return 0;
	}	
	printf ("qtd_atributos_rotulos: %d\n", base->qtd_atributos_rotulos);

	// Descobre quantos atributos realmente existem na base de dados
	base->qtd_atributos = base->qtd_atributos_rotulos - base->qtd_rotulos;
	printf("qtd_atributos: %d\n", base->qtd_atributos);


	/*base->qtd_atributos_rotulos = 11;
	base->qtd_atributos = 5;
	base->qtd_rotulos = 6;
	base->qtd_atributos_tirar = 2;*/

	// Aloca vetor referente as medias dos atributos omitidos
	base->medias = aloca_vetor_double(base->qtd_atributos);
	if (base->medias == NULL){
			printf("ocorreu um erro!\n");
			return 0;
	}
	zera_vetor_double(base->medias, base->qtd_atributos);
	base->qtd_elementos_medias = aloca_vetor(base->qtd_atributos);
	if (base->qtd_elementos_medias == NULL){
			printf("ocorreu um erro!\n");
			return 0;
	}
	zera_vetor(base->qtd_elementos_medias, base->qtd_atributos);

	escreve_parte1_arquivo(entrada, saida);	
	encontrar_instancias(entrada, saida, base, flag_base_binaria);
	percorre_parte1_arquivo(entrada, saida);
	imputacao_media (entrada, saida, base);
	
	fclose(entrada);
	fclose(saida);
	return 0;
}
