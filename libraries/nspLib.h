#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "strings.h"

typedef struct constraints{
	int *problem_size;
	int *number_of_assigments;
	int *consecutive_working_shifts;
	int **consecutive_assigments_matrix;
}Constraints;

typedef struct nspLib{
	int *problem_size;
	int **coverage_matrix; // alocar dinamicamente de acordo com o tamanho do problema (problem_size)
	int **preference_matrix; // alocar dinamicamente de acordo com a quantidade de enfermeiros(linhas) x (numero de dias*qnt de turnos) (colunas)
}NspLib;

void showVector(int *v);
void showConstraints(Constraints* c);
void showNsp(NspLib* nsp);

NspLib* readNspFile(char* file_name);
Constraints* readConstrainstsFile(char* url);

void freeNsp(NspLib* nsp);
void freeConstraints(Constraints* c);