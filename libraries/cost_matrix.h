#include<stdio.h>
#include<stdlib.h>
#include "nspLib.h"
#include "hungarian/hungarian.h"
#include "apc/apc.h"

extern int n_nurses;
extern int n_days;
extern int n_shifts;

extern bool useApcMultSolution;
extern bool useHungarianSolution;

#define MORNING 0 
#define EVENING 1 
#define NIGHT 2 
#define FREE 3 
#define ONE 1

#define MAX_SHIFTS 3

#define Ph 100
#define Ps 100

/**
* This struct save a schedule solution. The schedule is save in two matrix of lists.
* 	1 - nurse_per_day, save the relation between nurse(lines) and day(columns);
* 	2 - day_per_nurse, save the relation between day(lines) and nurses(columns)
* The content in this two matrix is the shift atributed to one nurse in a day.
*/
typedef struct{
	List** nurse_per_day;
	List** day_per_nurse;
	int cost_solution;
	int h_violations;
	int s_violations;
}Schedule;

/* Método util para copiar solução gerada*/
Schedule* copy_solution(Schedule* s);

/*Método utilizado para desalocar espaços na memoria*/
void free_schedule(Schedule* s);

/*Método utilizado para criar a primeira solução na fase construtiva*/
Schedule* buildFirstCostMatrix(NspLib* nsp, Constraints* c);

/*Método utilizado para calcular o custo das soluções obtidas*/
int cost_solution(Schedule* s, Constraints* c, NspLib* nsp);

/*Método utilizado para apresentar na tela o grafo gerado pelas soluções*/
void show_multipartite_graph(Schedule* m, int x);

/*Método utilizado para construir uma solução intermediria para otimizar no melhoramento*/
int constrants_cost_matrix_com_escala(int *_cost, int *hardConstraints, int *softConstraints,
                                      Schedule *s, NspLib *nsp, Constraints *c, Node *nurse, int days,
                                      int **minimum_coverage);

int verify_constraints(Constraints*c, Schedule* sc, int n);