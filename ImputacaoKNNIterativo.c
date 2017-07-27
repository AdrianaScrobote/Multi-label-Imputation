// Imputação utilizando KNN
// Algoritmo que utiliza a menor distância euclidiana em relação a instância com valores faltantes para realizar a imputação de valores

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>


typedef struct base{
	char nome[50];
	int qtd_atributos;
	int qtd_rotulos;
	int qtd_atributos_rotulos;
	int qtd_instancias;
	double ** base_de_dados;
	int ** base_auxiliar;
	double * medias;
	int * qtd_elementos_medias;
} Base;


// Aloca base
Base * alocarBase (){
	Base * base = (Base *) malloc (sizeof (Base));
	if (base != NULL){
		base->qtd_atributos = 0;
		base->qtd_rotulos = 0;
		base->qtd_atributos_rotulos = 0;
		base->qtd_instancias = 0;
		base->base_de_dados = NULL;
		base->base_auxiliar = NULL;
		return base;
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
void zera_vetor_double (double * vetor, int tamanho){
	if (vetor != NULL){
		int i;
		for (i = 0; i < tamanho; i++){
			vetor[i] = 0;
		}
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


// Imprime vetor de doubles
void imprimir_vetor_double (double * vetor, int tamanho){
	int i;
	for (i = 0; i < tamanho; i++){
		printf ("%d: %g\t", i, vetor[i]);
	}
	printf ("\n");
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

// Inicializa vetor de inteiros
void inicializa_vetor (int * vetor, int tamanho){
	if (vetor != NULL){
		int i;
		for (i = 0; i < tamanho; i++){
			vetor[i] = -1;
		}
	}	
}


// Imprime vetor de inteiros
void imprimir_vetor (int * vetor, int tamanho){
	int i;
	printf("\n");
	for (i = 0; i < tamanho; i++){
		printf ("%d: %d\n", i, vetor[i]);
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

 
// Recebe todas as instâncias e as carrega em uma matriz 
// -1 representa os valores ausentes
// valores completos são diferentes de -1 
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
void encontrar_instancias (FILE * arquivo, FILE * saida, Base * base, double ** base_de_dados){
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
				carrega_atributos_matriz (linha, base->qtd_atributos_rotulos, base_de_dados, contador);
				contador++;
			}
			//printf("contador: %d\n", contador);
		}
	}	
}

//Percorre parte inicial do arquivo fornecido
void percorre_parte1_arquivo(FILE * arquivo){
	if (arquivo != NULL){
		size_t tamanho = 100; //valor arbitrario
		char * linha = malloc(tamanho);
		if (linha != NULL){
			fseek(arquivo, 0, SEEK_SET);
			do{
				getline(&linha, &tamanho, arquivo);
			}
			while (strcmp (linha, "@data\n") != 0);
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



// Aloca matriz de inteiros
int ** alocar_matriz_int(int qtd_instancias, int qtd_atributos){
	int ** matriz = (int **) malloc (sizeof (int *) * qtd_instancias);
	if (matriz != NULL){
		int i;
		for (i = 0; i < qtd_instancias; i++){
			matriz[i] = (int *) malloc (sizeof (int) * qtd_atributos);
		}
	}
	return matriz;
}


// Inicializa elementos de uma matriz de inteiros com 1
// 1 pq equivale a valores completos que virão a ser incompletos (0)
void iniciar_matriz_int(int ** matriz, int qtd_instancias, int qtd_atributos){
	int i, j;
	for (i = 0; i < qtd_instancias; i++){
		for (j = 0; j < qtd_atributos; j++){
			matriz[i][j] = 1;
		}
	}
}

// Imprime valores de uma matriz de inteiros
void imprimir_matriz_int(int ** matriz, int qtd_instancias, int qtd_atributos){
	int i, j;
	for (i = 0; i < qtd_instancias; i++){
		for (j = 0; j < qtd_atributos; j++){
			printf ("%d\t", matriz[i][j]);
		}
		printf ("\n");
	}
}


// Conta e soma todos os atributos válidos para poder calcular a média
// Então calcula a média
void calcular_media (Base * base, int flag_base_binaria){
	int i, j, l;	
	for (l = 0; l < base->qtd_instancias; l++){
		for (i = base->qtd_rotulos; i < base->qtd_atributos_rotulos; i++){
			if (base->base_de_dados[l][i] != -1){
				base->medias[i - base->qtd_rotulos] += base->base_de_dados[l][i];
				base->qtd_elementos_medias[i -  base->qtd_rotulos] += 1;
			}
		}
	}
	for (i = 0; i < base->qtd_atributos; i++){
		base->medias[i] /= base->qtd_elementos_medias[i];
	}
	if (flag_base_binaria == 1){
		for (i = 0; i < base->qtd_atributos; i++){
			if (base->medias[i] > 0.5)		
				base->medias[i] = 1;
			else
				base->medias[i] = 0;
		}
	}
}


// Conta e soma todos os atributos válidos para poder calcular a média
// Então calcula a média
void calcular_media_k_menores (Base * base, int * posicao_menor_distancia, int k, int flag_base_binaria){
	int i, j, l;	
	zera_vetor_double(base->medias, base->qtd_atributos);
	for (l = 0; l < k; l++){
		for (i = base->qtd_rotulos; i < base->qtd_atributos_rotulos; i++){
				base->medias[i - base->qtd_rotulos] += base->base_de_dados[posicao_menor_distancia[l]][i];
		}
	}
	for (i = 0; i < base->qtd_atributos; i++){
		base->medias[i] /= k;
	}
	if (flag_base_binaria == 1){
		for (i = 0; i < base->qtd_atributos; i++){
			if (base->medias[i] > 0.5)		
				base->medias[i] = 1;
			else
				base->medias[i] = 0;
		}
	}
}

// matriz utilizada na imputação por KNN (2ª fase)
// matriz_auxiliar é para representar os valores ausentes e não ausentes que serão úteis após ocorrer a imputação por média (1ª fase) na matriz original
// 0 representa valores ausentes
// 1 representa valores não ausentes
void carregar_matriz_auxiliar(Base * base){
	int i, j;
	for (i = 0; i < base->qtd_instancias; i++){
		for (j = base->qtd_rotulos; j < base->qtd_atributos_rotulos; j++){
			if (base->base_de_dados[i][j] == -1){
				base->base_auxiliar[i][j - base->qtd_rotulos] = 0;
			}
		}
	}	
}

// Calcula a distância euclidiana entre duas instâncias
double calcula_distancia_euclidiana(double * instancia1, double * instancia2, int qtd_rotulos, int qtd_atributos_rotulos){
	double somatorio = 0;
	int i;
	double valor1, valor2;
	for (i = qtd_rotulos; i < qtd_atributos_rotulos; i++){
		valor1 = instancia1[i]; 
		valor2 = instancia2[i];
		if (valor1 == -1)
			valor1 = 0;
		if (valor2 == -1)
			valor2 = 0;
		somatorio += pow((valor1 - valor2), 2);
	}
	//printf("somatorio: %g\t", somatorio);
	return sqrt(somatorio);
}


//Substitui valores ausentes pela menor distância euclidiana encontrada
void ausentes_por_menor_distancia (Base * base, int posicao_instancia){
	int i;

	for (i = 0; i < base->qtd_atributos; i++){			
		if (base->base_auxiliar[posicao_instancia][i] == 0){
			base->base_de_dados[posicao_instancia][i + base->qtd_rotulos] = base->medias[i];
		}
	}
	
}

int encontra_maior(double * menor_distancia, int k){
	int i, maior = 0;
	for (i = 1; i < k; i++){
		if (menor_distancia[maior] < menor_distancia[i])
			maior = i;
	}
	return maior;
}


// Imputação utilizando KNN que substitui valores faltantes pela menor distãncia euclidiana encontrada
void encontra_k_vizinhos (double ** base_de_dados, int i, Base * base, int k, int flag_base_binaria){
	if (k > 0)
	{
		int j, l, qtd_preenchido = 0, maior_k_menores = 0;
		double distancia;
		double * menor_distancia = aloca_vetor_double(k);
		int * posicao_menor_distancia = aloca_vetor(k);

		for(j = 0; j < base->qtd_instancias; j++){
				if (i != j){
					distancia = calcula_distancia_euclidiana(base_de_dados[i], base_de_dados[j], base->qtd_rotulos, base->qtd_atributos_rotulos);
					for (l = 0; l < k; l++){
						if (menor_distancia[l] == -1){
							menor_distancia[l] = distancia;
							qtd_preenchido++;
							posicao_menor_distancia[l] = j;
							break;
						}
						else{
							if (distancia < menor_distancia[l] && qtd_preenchido > k-1){
						                maior_k_menores = encontra_maior(menor_distancia,k);						
								menor_distancia[maior_k_menores] = distancia;
								posicao_menor_distancia[maior_k_menores] = j;
								break;
							}
						}
					}
					
					//printf("[%d,%d] distancia: %g -> Menor0: %g	 Menor1: %g	 Menor2: %g\n", i, j, distancia, menor_distancia[0], menor_distancia[1], menor_distancia[2]);
					//printf("[%d,%d] distancia: %g -> Posica0: %d	 Posicao1: %d	 Posicao2: %d\n", i, j, distancia,posicao_menor_distancia[0], posicao_menor_distancia[1], posicao_menor_distancia[2]);
				}
		}
		//printf("[%d,%d] distancia: %g -> Posica0: %d	 Posicao1: %d	 Posicao2: %d\n", i, j, distancia,posicao_menor_distancia[0], posicao_menor_distancia[1], posicao_menor_distancia[2]);
		calcular_media_k_menores (base, posicao_menor_distancia, k, flag_base_binaria);
		//printf("vetor medias:\n");
		//imprimir_vetor_double (base->medias, base->qtd_atributos);
		free(menor_distancia);
		menor_distancia = NULL;
		free(posicao_menor_distancia);
		posicao_menor_distancia = NULL;
		
	}
}


// Imputação utilizando KNN que substitui valores faltantes pela menor distãncia euclidiana encontrada
void imputacao_KNN (double ** base_de_dados, Base * base, int k, int flag_base_binaria){
	if (k > 0)
	{
		int i, j, l, qtd_preenchido, maior_k_menores;
		double distancia;
		double * menor_distancia = aloca_vetor_double(k);
		int * posicao_menor_distancia = aloca_vetor(k);
		for (i = 0; i < base->qtd_instancias; i++){
		//for (i = 0; i < 1; i++){
			encontra_k_vizinhos (base_de_dados, i, base, k, flag_base_binaria);
			//printf("vetor medias:\n");
			//imprimir_vetor_double (base->medias, base->qtd_atributos);
			ausentes_por_menor_distancia(base, i);
		}
	}
}


void imputacao_media (Base * base){
	if (base != NULL){	
		int i, j;
		for (i = 0; i < base->qtd_instancias; i++){
			for (j = base->qtd_rotulos; j < base->qtd_atributos_rotulos; j++){
				if (base->base_de_dados[i][j] == -1)
					base->base_de_dados[i][j] = base->medias[j - base->qtd_rotulos];
			}
		}	
	}	
}


// Escreve todas as instâncias da base de dados
void escreve_parte2_arquivo(FILE * saida, double ** base_de_dados, Base * base){
	int i;
	for (i = 0; i < base->qtd_instancias; i++){
		char * instancia = (char*) malloc (sizeof (double) * base->qtd_atributos_rotulos * 2);
		if (instancia == NULL){
			printf("ocorreu um erro!\n");
			return;
		}
		vetor_to_instancia(base_de_dados[i], instancia, base->qtd_atributos_rotulos);
		fprintf(saida, "%s", instancia);
	}
}

int main(){ 
	char nome_arquivo_saida[70];

	int flag_base_binaria, k = 3;
	
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
	sprintf(nome_arquivo_saida, "%.*s_ImputacaoKNN.arff", tam_nome_arquivo-5, base->nome);

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
	
	base->base_auxiliar = alocar_matriz_int(base->qtd_instancias, base->qtd_atributos);
	iniciar_matriz_int(base->base_auxiliar, base->qtd_instancias, base->qtd_atributos);

	encontrar_instancias(entrada, saida, base, base->base_de_dados);
	//printf ("matriz carregada\n");
	//imprimir_matriz(base->base_de_dados, base->qtd_instancias, base->qtd_atributos_rotulos);
	carregar_matriz_auxiliar(base);
	//printf("matriz auxiliar\n");
	//imprimir_matriz_int(base->base_auxiliar, base->qtd_instancias, base->qtd_atributos);

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
	calcular_media(base, flag_base_binaria);
	printf ("Imputacao Media Concluida\nImputacao KNN em andamento ...\n");
	//printf("vetor medias:\n");
	//imprimir_vetor_double (base->medias, base->qtd_atributos);
	imputacao_media (base);
	//printf("matriz imputada media\n");
	//imprimir_matriz(base->base_de_dados, base->qtd_instancias, base->qtd_atributos_rotulos);
	
	percorre_parte1_arquivo(saida);	
	

	imputacao_KNN(base->base_de_dados, base, k, flag_base_binaria);
	//printf("matriz imputada knn\n");
	//imprimir_matriz(base->base_de_dados, base->qtd_instancias, base->qtd_atributos_rotulos);
	escreve_parte2_arquivo(saida, base->base_de_dados, base);
	
	fclose(entrada);
	fclose(saida);
	return 0;
}
