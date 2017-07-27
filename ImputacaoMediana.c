// Imputação pela mediana

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct base{
	char nome[50];
	int qtd_atributos;
	int qtd_rotulos;
	int qtd_atributos_rotulos;
	int qtd_instancias;
	double ** base_de_dados;
} Base;


typedef struct mediana {
	double * vetor_ordenado;
	double * valor_mediana;
} Mediana;

// Aloca Base
Base * alocarBase (){
	Base * base = (Base *) malloc (sizeof (Base));
	if (base != NULL){
		base->qtd_atributos = 0;
		base->qtd_rotulos = 0;
		base->qtd_atributos_rotulos = 0;
		base->qtd_instancias = 0;
		base->base_de_dados = NULL;
		return base;
	}
}

// Aloca Mediana
Mediana * alocarMediana(int qtd_instancias, int qtd_atributos){
	Mediana * mediana = (Mediana *) malloc (sizeof (Mediana));
	if (mediana != NULL){
		mediana->vetor_ordenado = (double *) malloc (sizeof (double) * qtd_instancias);
		if (mediana->vetor_ordenado != NULL){
			mediana->valor_mediana = (double *) malloc (sizeof (double) * qtd_atributos);
			if (mediana->valor_mediana != NULL)
				return mediana;
		}
	}
	return NULL;
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

// Descobre quantas instâncias existem na base de dados 
int descobrir_qtd_instancias(FILE * arquivo){
	int qtd_instancias = 0;
	size_t tamanho = 100; //valor arbitrario
	char * linha = malloc(tamanho);
	if (linha != NULL){
		while (getline(&linha, &tamanho, arquivo) > 0){
			qtd_instancias++; 
		}
		return qtd_instancias;
	}
	return -1;
}

// Escreve parte inicial do arquivo fornecido no arquivo de saída
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

// Aloca matriz de double
double ** alocar_matriz_double(int qtd_instancias, int qtd_atributos){
	double ** matriz = (double **) malloc (sizeof (double *) * qtd_instancias);
	if (matriz != NULL){
		int i;
		for (i = 0; i < qtd_instancias; i++){
			matriz[i] = (double *) malloc (sizeof (double) * qtd_atributos);
		}
	}
	return matriz;
}


// Inicializa elementos de uma matriz com -1
void iniciar_matriz(double ** matriz, int qtd_instancias, int qtd_atributos){
	int i, j;
	for (i = 0; i < qtd_instancias; i++){
		for (j = 0; j < qtd_atributos; j++){
			matriz[i][j] = -1;
		}
	}
}

// Imprime valores de uma matriz
void imprimir_matriz(double ** matriz, int qtd_instancias, int qtd_atributos){
	int i, j;
	for (i = 0; i < qtd_instancias; i++){
		for (j = 0; j < qtd_atributos; j++){
			printf ("%g\t", matriz[i][j]);
		}
		printf ("\n");
	}
}

// Recebe todas as instâncias e as carrega em uma matriz 
void carrega_atributos_matriz (char * linha, int qtd_atributos_rotulos, double ** base_de_dados, int contador){
	if (linha != NULL && qtd_atributos_rotulos > 0){
		int i = 0, j = 0, j_inicio = 0;
		char * valor;
		while (linha[j] != '\0'){
			if (linha[j] == ','){
				valor = encontrar_substring (linha, j_inicio, j - j_inicio);
				base_de_dados[contador][i] = atof(valor);
				free(valor);
				valor = NULL;
				i++;
				j++;

				// Verifica se o próximo atributo é ausente
				while (linha[j] == ','){
					valor = "-1";
					base_de_dados[contador][i] = atof(valor);
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
			base_de_dados[contador][i] = atof(valor);
		}
		else{
			valor = encontrar_substring (linha, j_inicio, j - 1 - j_inicio);
			base_de_dados[contador][i] = atof(valor);
		}
	}
}

// Encontra todas as instâncias e chama a função para armazená-las em uma matriz
void encontrar_instancias (FILE * arquivo, FILE * saida, Base * base){
	if (arquivo != NULL || base != NULL){
		int contador = 0;
		size_t tamanho = 100; //valor arbitrario
		char * linha = malloc(tamanho);
		//char * instancia = NULL;
	
		if (linha != NULL){			
			//PENSARRR MELHORRR
			char * instancia = (char*) malloc (sizeof (double) * base->qtd_atributos_rotulos * 2);
			if (instancia == NULL){
				printf("ocorreu um erro!\n");
				return;
			}
			while (getline(&linha, &tamanho, arquivo) > 0){
			//while (getline(&linha, &tamanho, arquivo) > 0 && contador < 6){
				carrega_atributos_matriz (linha, base->qtd_atributos_rotulos, base->base_de_dados, contador);
				contador++;
			}
			//printf("contador: %d\n", contador);
		}
	}	
}

void inicializa_vetor(int * vetor, int tamanho){
	int i;
	for (i = 0; i < tamanho; i++)
		vetor[i] = -1;
}

void inicializa_vetor_double(double * vetor, int tamanho){
	int i;
	for (i = 0; i < tamanho; i++)
		vetor[i] = -1;
}

void imprimir_vetor(int * vetor, int tamanho){
	int i;
	for (i = 0; i < tamanho; i++)
		printf("%d\t", vetor[i]);
	printf("\n");
}

void imprimir_vetor_double(double * vetor, int tamanho){
	int i;
	for (i = 0; i < tamanho; i++)
		printf("%f\t", vetor[i]);
	printf("\n");
}


// Comparação para deixar vetor crescente através da ordenação quick sort
int comparacao_crescente(const void * a, const void * b){
	if (*(double *) a == *(double *) b){
		return 0; //iguais
	}
	else if (*(double *) a < *(double *) b){
		return -1; //menor
	}
	else{
		return 1; //maior
	}
}


// Recebe um vetor ordenado e descobre a sua mediana de acordo com o tamanho do vetor
// Trata casos de bases binárias
double encontrar_indice_mediana(int tamanho, double * vetor_ordenado, int flag_base_binaria){
	double mediana;
	if (tamanho == 1)
		mediana = vetor_ordenado[0];
	else if (tamanho == 2)
		mediana = (vetor_ordenado[0] + vetor_ordenado[1]) / 2;
	else{
		if (tamanho % 2 == 0){
			tamanho = tamanho / 2;
			// verifica se mediana será dada por (0 + 1)/2, pois nesse caso o resultado será 0.5 o q não pode ocorrer em uma base binária
			if (flag_base_binaria == 1 && (vetor_ordenado[tamanho - 1] == 0 && vetor_ordenado[tamanho] == 1)){
				mediana = rand() % 2;				
			}
			else 
				mediana = (vetor_ordenado[tamanho] + vetor_ordenado[tamanho - 1]) / 2;
		}
		else{
			tamanho = tamanho / 2;
			mediana = vetor_ordenado[tamanho];
		}
	}
	return mediana;
}


// Encontra os valores na base de dados para fazer a ordenação do vetor e entao encontrar a mediana
void encontrar_valores_ordenacao(Base * base, Mediana * mediana, int flag_base_binaria){
	int i, j, l;
	double med;
	for (i = base->qtd_rotulos; i < base->qtd_atributos_rotulos; i++){
		inicializa_vetor_double(mediana->vetor_ordenado, base->qtd_instancias);
		l = 0, j = 0;
		for (j = 0; j < base->qtd_instancias; j++){		
			if (base->base_de_dados[j][i] != -1){
				// Encontra os valores e adiciona no vetor
				mediana->vetor_ordenado[l] = base->base_de_dados[j][i];
				l++;
			}
		}
		// Ordenacao quick sort
		qsort(mediana->vetor_ordenado, l, sizeof(double), comparacao_crescente);
		// Encontra mediana
		med = encontrar_indice_mediana(l, mediana->vetor_ordenado, flag_base_binaria);
		mediana->valor_mediana[i - base->qtd_rotulos] = med;
	}
	//printf ("mediana:\n");
	//imprimir_vetor_double(mediana->valor_mediana, base->qtd_atributos);
}

// Substitui os valores ausentes pelas respectivas medianas
void imputacao_mediana(Base * base, double * mediana){
	int i, j;
	for (i = 0; i < base->qtd_instancias; i++){
		for (j = base->qtd_rotulos; j < base->qtd_atributos_rotulos; j++){
			if(base->base_de_dados[i][j] == -1){
				base->base_de_dados[i][j] = mediana[j - base->qtd_rotulos];
			}
		}
	}
}

// Copia um vetor de doubles para um vetor de characteres (uma instância)
char* vetor_to_instancia(double * vetor, char * instancia, int qtd_atributos_rotulos){
	if (vetor != NULL && qtd_atributos_rotulos > 0){
		int i;
		char valor_atributo[20];
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


// Escreve todas as instâncias da base de dados
void escreve_parte2_arquivo(FILE * saida, Base * base){
	int i;
	for (i = 0; i < base->qtd_instancias; i++){
		char * instancia = (char*) malloc (sizeof (double) * base->qtd_atributos_rotulos * 2);
		if (instancia == NULL){
			printf("ocorreu um erro!\n");
			return;
		}
		vetor_to_instancia(base->base_de_dados[i], instancia, base->qtd_atributos_rotulos);
		fprintf(saida, "%s", instancia);
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
	sprintf(nome_arquivo_saida, "%.*s_ImputacaoMediana.arff", tam_nome_arquivo-5, base->nome);

	// Abre arquivo de saída
	FILE * saida = fopen(nome_arquivo_saida, "a+");
	if (saida == NULL){
		printf("Arquivo nao encontrado!\n");
		return 0;
	}

	// Descobre quantos rótulos existem na base de dados
	base->qtd_rotulos = descobrir_qtd_rotulos(entrada);
	printf("qtd_rotulos: %d\n", base->qtd_rotulos);

	// Descobre quantos atributos existem na base de dados (incluindo rótulos)
	base->qtd_atributos_rotulos = descobrir_qtd_atributos(entrada);
	if (base->qtd_atributos_rotulos == -1){
		printf("ocorreu um erro!\n");
		return 0;
	}	
	printf ("qtd_atributos_rotulos: %d\n", base->qtd_atributos_rotulos);

	// Descobre quantos atributos realmente existem na base de dados
	base->qtd_atributos = base->qtd_atributos_rotulos - base->qtd_rotulos;
	printf("qtd_atributos: %d\n", base->qtd_atributos);

	// Descobre quantas intâncias existem na base de dados
	base->qtd_instancias = descobrir_qtd_instancias(entrada);
	if (base->qtd_instancias == -1){
		printf("ocorreu um erro!\n");
		return 0;
	}
	printf("qtd_instancias: %d\n", base->qtd_instancias);
		
	/*base->qtd_atributos_rotulos = 11;
	base->qtd_atributos = 5;
	base->qtd_rotulos = 6;*/ 

	escreve_parte1_arquivo(entrada, saida);	

	base->base_de_dados = alocar_matriz_double(base->qtd_instancias, base->qtd_atributos_rotulos);
	iniciar_matriz(base->base_de_dados, base->qtd_instancias, base->qtd_atributos_rotulos);

	encontrar_instancias(entrada, saida, base);
	//imprimir_matriz(base->base_de_dados, base->qtd_instancias, base->qtd_atributos_rotulos);

	Mediana * mediana = alocarMediana(base->qtd_instancias, base->qtd_atributos);
	if (mediana == NULL){
		printf("houve um problema na alocacao\n");
		return 0;
	}

	inicializa_vetor_double(mediana->vetor_ordenado, base->qtd_instancias);
	encontrar_valores_ordenacao(base, mediana, flag_base_binaria);
	//printf ("mediana:\n");
	//imprimir_vetor_double(mediana->valor_mediana, base->qtd_atributos);
	imputacao_mediana(base, mediana->valor_mediana);
	escreve_parte2_arquivo(saida, base);

	return 0;
}
