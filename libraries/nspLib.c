#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "strings.h"

extern int n_nurses;
extern int n_days;
extern int n_shifts;

typedef struct constraints {
    int *problem_size;
    int *number_of_assigments;
    int *consecutive_working_shifts;
    int **consecutive_assigments_matrix; // consecutive same working shifts and number of assigments per shift
} Constraints;

typedef struct nspLib {
    int *problem_size;
    int **coverage_matrix; // alocar dinamicamente de acordo com o tamanho do problema (problem_size)
    int **preference_matrix; // alocar dinamicamente de acordo com a quantidade de enfermeiros(linhas) x (numero de dias*qnt de turnos) (colunas)
} NspLib;


int getValoresIntermediario(int* pSize, char* line, int i, int j);
int getValoresMaxAndMin(int* pSize1, int* pSize2, char* line);

void showVector(int *v) {
    for (int i = 0; i < 4; ++i) {
        printf("%d ", *(v + i));
    }
    printf("\n");
}

void showNsp(NspLib* nsp) {
    printf("Problem Size: %d , %d, %d \n", nsp->problem_size[0], nsp->problem_size[1], nsp->problem_size[2]);

    printf("Coverage Matrix\n");
    for (int i = 0; i < nsp->problem_size[1]; ++i) {
        for (int j = 0; j < nsp->problem_size[2]; j++) {
            printf("%d ", nsp->coverage_matrix[i][j]);
        }
        printf("\n");
    }

    printf("Preference Matrix\n");
    for (int i = 0; i < nsp->problem_size[0]; ++i) {
        for (int j = 0; j < (nsp->problem_size[2] * nsp->problem_size[1]); j++) {
            printf("%d ", nsp->preference_matrix[i][j]);
        }
        printf("\n");
    }
}

void showConstraints(Constraints* constraintsLocalShowContraints) {
    printf("Problem Size: %d , %d \n", constraintsLocalShowContraints->problem_size[0], constraintsLocalShowContraints->problem_size[1]);
    printf("Number of assigments: %d , %d \n", constraintsLocalShowContraints->number_of_assigments[0], constraintsLocalShowContraints->number_of_assigments[1]);
    printf("Consecutive working shifts: %d , %d \n", constraintsLocalShowContraints->consecutive_working_shifts[0], constraintsLocalShowContraints->consecutive_working_shifts[1]);

    for (int i = 0; i < constraintsLocalShowContraints->problem_size[1]; ++i) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", constraintsLocalShowContraints->consecutive_assigments_matrix[i][j]);
        }
        printf("\n");
    }
}

void readLine(char* line, int* vector) {
    int j = 0, s = 0, x = 0;
    int value = strlen(line);
    for (int i = 0; i < value; i++)
        if (line[i] == TAB) {
            vector[x] = strncopy(line, j, i);
            x++;
            s = i;
            j = i + 1;
        }


    vector[x] = strncopy(line, s + 1, strlen(line) - 1);
}

NspLib* readNspFile(char* file_name) {
    NspLib* nsp = (NspLib*) calloc(1, sizeof (NspLib));

    int index_c_m = 0; // index coverage matrix
    int index_p_m = 0; // index preference matrix

    FILE* arq = fopen(file_name, "rt");
    char* line = (char*) calloc(100000, sizeof (char));
    int n_line = 0;
    if (!arq) {
        perror("Can't open file.");
        exit(1);
    } else {
        while (!feof(arq)) {
            fgets(line, 255, arq);

            if (n_line == 0) { // first line. (problem size matix)

                nsp->problem_size = (int*) calloc(4, sizeof (int));
                readLine(line, nsp->problem_size);

                nsp->preference_matrix = (int**) calloc(nsp->problem_size[0] + 1, sizeof (int*));
                nsp->coverage_matrix = (int**) calloc(nsp->problem_size[1] + 1, sizeof (int*));

            }

            if (n_line >= 2 && n_line <= nsp->problem_size[1] + 1) {// coverage matrix
                nsp->coverage_matrix[index_c_m] = (int*) calloc(nsp->problem_size[2] + 1, sizeof (int));
                readLine(line, nsp->coverage_matrix[index_c_m]);
                index_c_m++;
            }

            int parametro = nsp->problem_size[1] + 3;
            if (n_line >= (parametro)) {
                nsp->preference_matrix[index_p_m] = (int*) calloc(((nsp->problem_size[1] * nsp->problem_size[2]) + 1), sizeof (int));
                readLine(line, nsp->preference_matrix[index_p_m]);
                index_p_m++;
            }
            n_line++;
        }

        fclose(arq);
        printf("Success read nsp file!\n\n");
    }
    free(line);
    return nsp;
}

Constraints* readConstrainstsFile(char* url) {

    FILE* arq = fopen(url, "rt");
    char line[1000];
    int n_line = 0, index_ca_m = 0;
    Constraints *constraintsLocalReadFile = (Constraints*) calloc(1, sizeof (Constraints));
    if (!arq) {
        perror("Can't open file.");
        exit(1);
    } else {
        while (!feof(arq)) {
            fgets(line, 255, arq);
            if (n_line == 0) { // problem size
                constraintsLocalReadFile->problem_size = (int*) calloc(3, sizeof (int));
                getValoresMaxAndMin(&constraintsLocalReadFile->problem_size[0],
                        &constraintsLocalReadFile->problem_size[1], line);

                constraintsLocalReadFile->consecutive_assigments_matrix = 
                        (int**) calloc(constraintsLocalReadFile->problem_size[1] + 1, sizeof (int*));
            }

            if (n_line == 2) { // number of assigments (min and max)
                constraintsLocalReadFile->number_of_assigments = (int*) calloc(3, sizeof (int));
                getValoresMaxAndMin(&constraintsLocalReadFile->number_of_assigments[0],
                        &constraintsLocalReadFile->number_of_assigments[1], line);
            }

            if (n_line == 4) { // consecutive working shifts (min nd max)
                constraintsLocalReadFile->consecutive_working_shifts = (int*) calloc(3, sizeof (int));
                getValoresMaxAndMin(&constraintsLocalReadFile->consecutive_working_shifts[0],
                        &constraintsLocalReadFile->consecutive_working_shifts[1], line);
            }
            int size = constraintsLocalReadFile->problem_size[1];
            if (n_line > 6 && n_line <= (6 + size)) {
                constraintsLocalReadFile->consecutive_assigments_matrix[index_ca_m] = (int*) calloc(5, sizeof (int));

                int endValor = getValoresMaxAndMin(&constraintsLocalReadFile->consecutive_assigments_matrix[index_ca_m][0],
                        &constraintsLocalReadFile->consecutive_assigments_matrix[index_ca_m][1], line);
                
                endValor = getValoresIntermediario(&constraintsLocalReadFile->consecutive_assigments_matrix[index_ca_m][2],
                        line, (endValor + 2), (endValor + 1));
                
                getValoresIntermediario(&constraintsLocalReadFile->consecutive_assigments_matrix[index_ca_m][3],
                        line, (endValor + 2), (endValor + 1));

                readLine(line, constraintsLocalReadFile->consecutive_assigments_matrix[index_ca_m]);
                index_ca_m++;
            }

            n_line++;
        }


        fclose(arq);
        printf("Success read constraints file!\n\n");
        return constraintsLocalReadFile;
    }
}

int getValoresMaxAndMin(int* pSize1, int* pSize2, char* line) {
    if (strncopy(line, 1, 1) == -1) {
        *pSize1 = strncopy(line, 0, 0);
        return getValoresIntermediario(pSize2, line, 3, 2);
    } else {
        *pSize1 = strncopy(line, 0, 0) * 10 + strncopy(line, 1, 1);
        return getValoresIntermediario(pSize2, line, 4, 3);
    }   
}

int getValoresIntermediario(int* pSize, char* line, int i, int j) {
    if (strncopy(line, i, i) == -1) {
        *pSize = strncopy(line, j, j);
        return i;
    } else {
        *pSize = strncopy(line, j, j) * 10 + strncopy(line, i, i);
        return i + 1;
    }
}

void freeConstraints(Constraints* constraintsLocalFreeConstraints) {
    free(constraintsLocalFreeConstraints->problem_size);
    free(constraintsLocalFreeConstraints->number_of_assigments);
    free(constraintsLocalFreeConstraints->consecutive_assigments_matrix);

    free(constraintsLocalFreeConstraints->consecutive_working_shifts);
    for (int i = 0; i < n_shifts; i++) {
        free(constraintsLocalFreeConstraints->consecutive_assigments_matrix[i]);
    }
    free(constraintsLocalFreeConstraints);
}

void freeNsp(NspLib* nsp) {
    free(nsp->problem_size);
    for (int i = 0; i < n_days; i++) {
        free(nsp->coverage_matrix[i]);
    }
    free(nsp->coverage_matrix);
    for (int i = 0; i <= n_nurses; i++) {
        free(nsp->preference_matrix[i]);
    }
    free(nsp->preference_matrix);
    free(nsp);
}

