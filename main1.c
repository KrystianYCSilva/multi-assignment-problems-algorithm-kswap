#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
#include <dirent.h>
#include "libraries/operators.h"

// Função principal do MAPA original Sem K-swap ou BeackingTraking
// =====================================================================================================================================================================
// ===                                                                                                                                                               ===     
// ===                                                                                                                                                               ===     
// ===                                                             Função principal do MAPA original                                                                 ===     
// ===                                                               Sem K-swap ou BeackingTraking                                                                   ===     
// ===                                                                                                                                                               ===           
// ===                                                                                                                                                               ===           
// =====================================================================================================================================================================


int it = 3; // Iterações acima de 10 somente são significativas para instancias realistas, as demais convergem em torno da 5 iteração

//Tem comentario no codigo na linha 58 e 68 pra executar ok ? qualquer duvida fala

// Parametros NSPLib
int n_nurses = 25;
int n_days = 7;
int n_shifts = 4;
int max_files = 7290; //para 25-50-75-100
//int max files = 690; //para 30-60

// Algoritmos 
bool useApcMultSolution = true;
bool useHungarianSolution = false;

// Parametros Documentos
const char *diretorioBiblioteca = "./files/N25";
const char *pastaBiblioteca = "files/N25/";
const char *pastaConstranits = "files/casos-1-8/";
const int arqConstraint = 1;

//Constante Para indexar o salvar
const char *funcaoExecutada = "Constraint_";
const char *diretorioSave = "resultados/N25/";

Constraints *constraintsGlobal;

Schedule *multi_assignment_procedures(Schedule *initial_s, NspLib *nsp) {
    printf("Iterations: %d", it);
    Schedule *previous_s = copy_solution(initial_s);
    Schedule *best_s = copy_solution(initial_s);

    int i = 0, k = 1, pause = 0;
    previous_s->cost_solution = cost_solution(previous_s, constraintsGlobal, nsp);
    Schedule *current_s = copy_solution(previous_s);
    printf("-- Entrou do While  --------------------------------- ");
    Schedule *mim_sol = copy_solution(current_s);
    while (i != it) {
        Schedule *s_line = copy_solution(current_s);
        Schedule *pause_sol = copy_solution(current_s);

        s_line = pcr(s_line, nsp, constraintsGlobal);
        s_line = kSwap(s_line, nsp, constraintsGlobal, k);
        s_line = pcr_backward(s_line, nsp, constraintsGlobal);
        s_line = kSwap_backward(s_line, nsp, constraintsGlobal, k);

        s_line = kSwap(s_line, nsp, constraintsGlobal, 4);
        s_line = kSwap(s_line, nsp, constraintsGlobal, 3);
        s_line = kSwap(s_line, nsp, constraintsGlobal, 2);

        s_line->cost_solution = cost_solution(s_line, constraintsGlobal, nsp);
        if (current_s->cost_solution == s_line->cost_solution) {
            i = i + 1;
        } else {
            i = 0;
            current_s = copy_solution(s_line);
        }
        if (mim_sol->cost_solution >= s_line->cost_solution)
            mim_sol = copy_solution(s_line);
        if (pause_sol->cost_solution >= s_line->cost_solution)
            pause_sol = copy_solution(s_line);
        if (s_line->cost_solution == pause_sol->cost_solution) {
            pause++;
        }
        if (pause == it)
            i = it;
        if (i == it) {
            current_s = copy_solution(mim_sol);
            free_schedule(s_line);
            free_schedule(mim_sol);
            free_schedule(pause_sol);
        }
    }
    if (current_s->cost_solution == previous_s->cost_solution) {
        printf("Solução Anterior( %d ) igual a Atual: %d\n", previous_s->cost_solution, current_s->cost_solution);
        printf("Numero de repetições: %d de %d \n", i, it);
    } else {
        printf("Solução Anterior( %d  ) diferente da Atual: %d\n", previous_s->cost_solution, current_s->cost_solution);
        if (current_s->cost_solution <= previous_s->cost_solution) {
            if (current_s->cost_solution <= best_s->cost_solution) {
                free_schedule(best_s);
                best_s = copy_solution(current_s);
                printf("-- (else if current < best) Cost: %d (it: %d)\n", best_s->cost_solution, i);
            }
        } else {
            printf("Current:::: %d\n", current_s->cost_solution);
            printf("-- (else) Cost: %d (it: %d)\n", best_s->cost_solution, i);
            if (previous_s->cost_solution <= best_s->cost_solution) {
                free_schedule(best_s);
                best_s = copy_solution(previous_s);
            }
        }
        previous_s = copy_solution(current_s);
    }
    free_schedule(current_s);
    initial_s = copy_solution(best_s);

    printf("-- Saiu do While  ---------------------------------");
    printf("-- Cost: %d (it: %d)\n", best_s->cost_solution, i);

    free_schedule(previous_s);
    free_schedule(best_s);
    return initial_s;
}

void saveDatas(char *name, char *name1, char *constraint, int it, Schedule *s, int initial_cost, double t) {
    FILE *fp;

    fp = fopen(name, "a+");

    if (fp == NULL)
        printf("nao abriu\n");

    fprintf(fp, "%s,%s;%d;%d;%d;%d;%d,%f\n"
            , name1, constraint, it, initial_cost, s->cost_solution, s->h_violations,s->s_violations, t);
    fclose(fp);
}

NspLib *readAndSaveNsp(char *dName) {
    printf("Diretorio Aberto:  %s", dName);
    //Le arquivo de exercicio
    char *realPath = (char *) calloc(256, sizeof(char));
    realPath = strcat(realPath, pastaBiblioteca);
    strcat(realPath, dName);
    NspLib *nsp = readNspFile(realPath);

    // Mostrar NSP lido
    showNsp(nsp);

    free(realPath);
    return nsp;
}

void readAndSaveConstraints(char *fileName) {

    char *realPath = (char *) calloc(256, sizeof(char));
    realPath = strcat(realPath, pastaConstranits);
    realPath = strcat(realPath, fileName);
    //Leitura das Costraints
    constraintsGlobal = readConstrainstsFile(realPath);
    // Mostrar Constrants lido
    showConstraints(constraintsGlobal);
}

char *criarNomeArquivoResultado(char *fileName) {
    char *saveAt = (char *) calloc(256, sizeof(char));

    //Cria Nome do arquivo de resultados
    saveAt = strcat(saveAt, diretorioSave);
    saveAt = strcat(saveAt, funcaoExecutada);
    saveAt = strcat(saveAt, fileName);
    strcat(saveAt, ".cvs");
    return saveAt;
}

Schedule *faseConstrutiva(NspLib *nsp) {
    Schedule *primeiraSolucao = (Schedule *) calloc(1, sizeof(Schedule));
    if (useHungarianSolution) {
        primeiraSolucao = buildFirstCostMatrix(nsp, constraintsGlobal);
        primeiraSolucao->cost_solution = cost_solution(primeiraSolucao, constraintsGlobal, nsp);
    }
    if (useApcMultSolution) {
        primeiraSolucao = buildFirstCostMatrix(nsp, constraintsGlobal);
        primeiraSolucao->cost_solution = cost_solution(primeiraSolucao, constraintsGlobal, nsp);
    }
    return primeiraSolucao;
}

int main() {

    //Leitura das Costraints
    char buf1[3];
    char *constraints = (char *) calloc(256, sizeof(char));
    snprintf(buf1, 3, "%d", arqConstraint);
    constraints = strcat(constraints, buf1);
    constraints = strcat(constraints, ".gen");
    readAndSaveConstraints(constraints);
    char *saveAt_result = criarNomeArquivoResultado(constraints);

    //Pega o primeiro exercicio e entra no loop para executar os exercicios do diretorio
    for (int file = 1910; file <= max_files; file++) {
        char buf3[10];
        char buf2[10];
        snprintf(buf3, 10,"/%d.nsp", file);
        snprintf(buf2, 10,"%d.nsp", file);
        char* path = (char*) calloc(256, sizeof (char));
        path = strcat(path, buf3);
        char* name_file = (char*) calloc(256, sizeof (char));
        name_file = strcat(name_file, buf2);

        clock_t tic = clock();
        NspLib *nsp = readAndSaveNsp(path);
        Schedule *primeraSolucao = faseConstrutiva(nsp);


        Schedule *solucaoMelhoramento = copy_solution(primeraSolucao);

        solucaoMelhoramento = multi_assignment_procedures(solucaoMelhoramento, nsp);
        clock_t toc = clock();


        double executed = (double) (toc - tic) / CLOCKS_PER_SEC;


        char *saveAt = (char *) calloc(256, sizeof(char));
        strcat(saveAt, name_file);

        solucaoMelhoramento->cost_solution = cost_solution(solucaoMelhoramento, constraintsGlobal, nsp);
        saveDatas(saveAt_result, saveAt, constraints, it, solucaoMelhoramento, primeraSolucao->cost_solution, executed);


        free(saveAt);
        freeNsp(nsp);
        free_schedule(solucaoMelhoramento);
        free_schedule(primeraSolucao);
    }

    freeConstraints(constraintsGlobal);
}