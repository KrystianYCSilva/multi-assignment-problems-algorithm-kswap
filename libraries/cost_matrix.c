#include "cost_matrix.h"

#define INF (0x7FFFFFFF)

void show_multipartite_graph(Schedule* m, int x) {
    if (x == 1) {
        for (int i = 0; i < n_days; i++) {
            printf("\nDay %d 	", i);
            printList(m->day_per_nurse[i]);
        }
    } else if (x == 0) {
        for (int i = 0; i < n_nurses; i++) {
            printf("\nNurse %d 	", i);
            printList(m->nurse_per_day[i]);
        }
    }
    printf("\n");
}

void free_schedule(Schedule* s) {
    for (int i = 0; i < n_days; i++) {
        freeList(s->day_per_nurse[i]);
        free(s->day_per_nurse[i]);
    }
    free(s->day_per_nurse);

    for (int i = 0; i < n_nurses; i++) {
        freeList(s->nurse_per_day[i]);
        free(s->nurse_per_day[i]);
    }
    free(s->nurse_per_day);
    free(s);
}

Schedule* copy_solution(Schedule* s) {
    Schedule* rt = (Schedule*) calloc(1, sizeof (Schedule));

    List** nurse_per_day = (List**) calloc(n_nurses, sizeof (List*));
    List** day_per_nurse = (List**) calloc(n_days, sizeof (List*));

    for (int i = 0; i < n_nurses; i++) {
        nurse_per_day[i] = (List*) calloc(n_days, sizeof (List));
        nurse_per_day[i] = copyList(s->nurse_per_day[i], nurse_per_day[i]);
    }

    for (int i = 0; i < n_days; i++) {
        day_per_nurse[i] = (List*) calloc(n_nurses, sizeof (List));
        day_per_nurse[i] = copyList(s->day_per_nurse[i], day_per_nurse[i]);
    }

    rt->nurse_per_day = nurse_per_day;
    rt->day_per_nurse = day_per_nurse;
    rt->cost_solution = s->cost_solution;

    return rt;
}

// =====================================================================================================================================================================
// ===                                                                                                                                                               ===     
// ===                                                                                                                                                               ===     
// ===                                                             Calculo das Restrições                                                                            ===     
// ===                                                            Quando ja existe solução                                                                           ===     
// ===                                                                                                                                                               ===           
// ===                                                                                                                                                               ===           
// =====================================================================================================================================================================

int sequencial_shifts(List* nurse_per_day, int* vet);

int* shifts_per_nurse(List** nurse_per_day);
int** shifts_per_day_matrix(List** day_per_nurse);
int* shifts_per_day_vector(List** nurse_per_day, int nurse);

int consecutive_work_shift(List** nurse_per_day, int nurse);
int* consecutive_same_work_shift(List** nurse_per_day, int nurse);

int verify_number_of_assigments(int shift_per_nurse, int* number_of_assigments);
int verify_consecutive_assigments(int consecutive_assigments, int min, int max);
int verify_minimum_coverage(int day, int** coverage_matrix, int shift, int nurse_shift);

int cost_solution(Schedule* s, Constraints* c, NspLib* nsp) {
    int** minimum_coverage = shifts_per_day_matrix(s->day_per_nurse);
    int** same_assignments = (int**) calloc(n_nurses, sizeof (int*));
    int* shift_per_nurse = shifts_per_nurse(s->nurse_per_day);

    int s_cost = 0;
    int total_cost = 0;
    int sequencialShift = 0;
    int numberAssigments = 0;
    int consecutiveAssigments = 0;
    int consecutiveWork = 0;
    int consecutiveSameWork = 0;

    s->h_violations = 0;
    s->s_violations = 0;

    printf("\n");
    for (int days = 0; days < n_days; days++) {
        List* day_schedule = s->day_per_nurse[days];
        Node* nurse = day_schedule->first;
        int index_n = 0, cost = 0;
        while (nurse != NULL) {
            int nHCV = 0, nSCV = 0;
            int vet = 1;
            if (days == 0) {
                nHCV = sequencial_shifts(s->nurse_per_day[index_n], &vet);
                if (nHCV != 0) {
                    sequencialShift++;
                }

                if (verify_number_of_assigments(vet, c->number_of_assigments) == 1) {
                    nHCV++;
                    numberAssigments++;
                }
            }
            int* assignments_shift = shifts_per_day_vector(s->nurse_per_day, index_n);
            for (int a = 0; a < n_shifts; a++) {
                if (assignments_shift[a] > c->consecutive_assigments_matrix[3][a]
                        && assignments_shift[a] < c->consecutive_assigments_matrix[2][a]) {
                    nHCV++;
                    consecutiveAssigments++;
                }
            }
            int consecutive_work_shifts = consecutive_work_shift(s->nurse_per_day, index_n);
            if (consecutive_work_shifts > c->consecutive_working_shifts[1] && consecutive_work_shifts < c->consecutive_working_shifts[0]) {
                nHCV++;
                consecutiveWork++;
            }
            int* consecutive_same_work_shifts = consecutive_same_work_shift(s->nurse_per_day, index_n);
            for (int a = 0; a < n_shifts; a++) {
                if (consecutive_same_work_shifts[a] > c->consecutive_assigments_matrix[a][1]
                        && consecutive_same_work_shifts[a] < c->consecutive_assigments_matrix[a][0]) {
                    nHCV++;
                    consecutiveSameWork++;
                }
            }

            if (verify_minimum_coverage(days, nsp->coverage_matrix, nurse->data, minimum_coverage[days][nurse->data])) {
                nSCV++;
            }

            //            if (nHCV != 0 || nSCV != 0)
            s_cost += nsp->preference_matrix[index_n][(days * n_shifts) + nurse->data];
            cost += nsp->preference_matrix[index_n][(days * n_shifts) + nurse->data] + Ph * nHCV + Ps * nSCV;
            s->h_violations += nHCV;
            s->s_violations += nSCV;
            index_n++;
            nurse = nurse->next;
        }
        total_cost += cost;
        //        printf("day: %d; cost: %d (nHCV: %d; nSCV: %d)\n", days, cost, s->h_violations, s->s_violations);
        printf("Dia %d - ", days);
        printList(s->day_per_nurse[days]);
        printf("\n");
    }
    printf("h_violations: %d, s_violations: %d, S_cost: %d ,cost: %d\n", s->h_violations, s->s_violations, s_cost, total_cost);
    printf("Consecutive work violation: %d\n", consecutiveWork);
    printf("Consecutive same work violation: %d\n", consecutiveSameWork);
    printf("Consecutive assigments violation: %d\n", consecutiveAssigments);
    printf("Number of assigments violation: %d\n", numberAssigments);
    printf("Sequencial shift violation: %d\n", sequencialShift);

    for (int i = 0; i < n_nurses; ++i) {
        free(same_assignments[i]);
    }
    free(same_assignments);

    for (int i = 0; i < n_days; i++) {
        free(minimum_coverage[i]);
    }
    free(minimum_coverage);
    free(shift_per_nurse);

    s->cost_solution = total_cost;
    return total_cost;
}

int* shifts_per_nurse(List** nurse_per_day) {
    int *rt = (int*) calloc(n_nurses, sizeof (int));

    for (int i = 0; i < n_nurses; i++) {
        Node* aux = nurse_per_day[i]->first;
        while (aux != NULL) {
            if (aux->data != FREE)
                rt[i]++;
            aux = aux->next;
        }
    }
    return rt;
}

int** shifts_per_day_matrix(List** day_per_nurse) {
    int** rt = (int**) calloc(n_days, sizeof (int*));

    for (int d = 0; d < n_days; d++) {
        rt[d] = (int*) calloc(n_shifts, sizeof (int));
        List* nurses = day_per_nurse[d];
        Node* e = nurses->first;

        while (e != NULL) {
            if (e->data == MORNING)
                rt[d][MORNING]++;
            if (e->data == EVENING)
                rt[d][EVENING]++;
            if (e->data == NIGHT)
                rt[d][NIGHT]++;
            if (e->data == FREE)
                rt[d][FREE]++;
            e = e->next;
        }
    }
    return rt;
}

int* shifts_per_day_vector(List** nurse_per_day, int nurse) {
    int* rt = (int*) calloc(n_shifts, sizeof (int));
    Node* e = nurse_per_day[nurse]->first;
    while (e != NULL) {
        if (e->data == MORNING) {
            rt[MORNING]++;
        }
        if (e->data == EVENING) {
            rt[EVENING]++;
        }
        if (e->data == NIGHT) {
            rt[NIGHT]++;
        }
        if (e->data == FREE) {
            rt[FREE]++;
        }
        e = e->next;
    }
    return rt;
}

int sequencial_shifts(List* nurse_per_day, int* vet) {
    int cont = 0;
    Node* node = nurse_per_day->first;
    while (node->next != NULL) {
        Node* aux = node->next;
        int cont = 0;
        if (node->data == EVENING && aux->data == MORNING)
            cont++;
        else if (node->data == NIGHT && aux->data == MORNING)
            cont++;
        else if (node->data == NIGHT && aux->data == EVENING)
            cont++;


        if (node->data == aux->data)
            cont++;

        if (node->data != FREE)
            (*vet)++;

        node = node->next;
    }
    return cont;
}

int consecutive_work_shift(List** nurse_per_day, int nurse) {
    int rt = 0;
    Node* e = nurse_per_day[nurse]->first;
    while (e != NULL) {
        int count = 0;
        Node* aux = e;
        while (aux != NULL && aux->data != FREE) {
            count++;
            aux = aux->next;
        }
        if (rt < count) {
            rt = count;
        }
        e = e->next;
    }
    return rt;
}

int* consecutive_same_work_shift(List** nurse_per_day, int nurse) {
    int* rt = (int*) calloc(n_shifts, sizeof (int));
    Node* e = nurse_per_day[nurse]->first;
    while (e != NULL) {
        int count = 0;
        Node* aux = e;
        while (aux != NULL && aux->data == e->data) {
            count++;
            aux = aux->next;
        }
        if (rt[e->data] < count) {
            rt[e->data] = count;
        }
        e = e->next;
    }
    return rt;
}

int verify_minimum_coverage(int day, int** coverage_matrix, int shift, int nurse_shift) {
    if (nurse_shift < coverage_matrix[day][shift]) {
        //        printf("nurser_per_shift: %d \n", nurse_shift);
        return 1;
    } else
        return 0;
}

int verify_consecutive_assigments(int consecutive_assigments, int min, int max) {
    if (consecutive_assigments > max)
        return 1;
    else if (consecutive_assigments < min)

        return 1;
    return 0;
}

int verify_number_of_assigments(int shift_per_nurse, int* number_of_assigments) {
    if (shift_per_nurse >= number_of_assigments[0] && shift_per_nurse <= number_of_assigments[1]) {
        return 0;
    } else {

        return 1;
    }
}

// =====================================================================================================================================================================
// ===                                                                                                                                                               ===     
// ===                                                                                                                                                               ===     
// ===                                                             CONSTRUÇÃO DA MATRIZ                                                                              ===     
// ===                                                               Fase Construtiva                                                                                ===     
// ===                                                                                                                                                               ===           
// ===                                                                                                                                                               ===           
// =====================================================================================================================================================================

// ***************************************** Calculo da Primeira matriz de custo **************************************************
int verifyShift(int shift, int previousShift);
int verifySameAssigment(int sameAssigments, int min, int max);
//int verifyNumberOfAssigment(int numberAssingment, int min, int max);
int verifyConsecutiveWorking(int consecutiveWorking, int min, int max);
int verifyConsecutiveWorkingPerShift(int consecutiveWorking, int min, int max);
int consecutiveWorkShift_(int daySchadule, int dayProxSchadule, int _nurse, int* numberOfConsecutiveWork);

void constraints_cost_matrix(NspLib* nsp, Constraints* c, int day, int nurse, int** cost,
        int** assigment, List** dayPerNurse, int** numberOfConsecutiveWorking, int* numberOfFrees,
        int** numberOfSameAssigment, int* numberOfConsecutiveWork);

// ***************************************** Calculo da Primeira matriz de custo **************************************************
int numberFreeShift(int min, int max);
int numberFreeShiftPerNurse(int min, int max);

void assingNurseToShift(int* vet, int shift);
int verificaRestricao(int shift, int previousShift);

void setMinimumCoverage(int* turnosNurse, int day, int nurse, int** coverage_matrix,
        int** preference_matrix, int** cost, int** assigment, int freeShifts, int* numberOfNursesPerShift);
void assignmentFreeShifts(int* turnos_nurse, int day, int nurse, int** preference_matrix,
        int** cost, int** assigment, int freeShifts, int* numberOfNursesPerShift, int* numberOfFrees);
void assignmentMinShiftsFirstDay(int* turnos_nurse, int day, int nurse, int** preference_matrix,
        int** cost, int** assigment, int* numberOfNursesPerShift, int* numberOfFrees);
void assignmentMinShifts(int* turnos_nurse, int day, int nurse, int** preference_matrix,
        int previousShift, int** cost, int** assigment, int* numberOfNursesPerShift, int* numberOfFrees);

int combineScheduleHungarian(int day, NspLib* nsp, int** assigment, List** dayPerNurse,
        List** nursePerDay, hungarian_problem_t* hungarian, int* numberOfFrees);
int combineScheduleApc(int day, NspLib* nsp, int** assigment, List** dayPerNurse,
        List** nursePerDay, apc_problem_t* apc, int* numberOfFrees);

int executeProceduteMultAssigmentHungarian(int d, NspLib* nsp, int** cost, int _cost,
        int** assigment, List** dayPerNurse, List** nursePerDay, int* numberOfFrees);
int executeProceduteMultAssigmentApc(int d, NspLib* nsp, int** cost, int _cost, Constraints* c,
        int** assigment, List** dayPerNurse, List** nursePerDay, int* numberOfFrees);


// =====================================================================================================================================================================
// ===                                                                                                                                                               ===     
// ===                                                                                                                                                               ===     
// ===                                                               Calculo de Restrições                                                                           ===     
// ===                                                              Para a Fase Construtiva                                                                          ===           
// ===                                                                                                                                                               ===           
// ===                                                                                                                                                               ===           
// =====================================================================================================================================================================

int verifyShift(int shift, int previousShift) {
    int count = 0;
    if (previousShift == EVENING && shift == MORNING)
        count = 1;
    if (previousShift == NIGHT && shift == MORNING)
        count = 1;
    if (previousShift == NIGHT && shift == EVENING)
        count = 1;
    return count;
}

/** 
 * min = c->consecutive_assigments_matrix[shift][2]
 * max = c->consecutive_assigments_matrix[shift][3]
 **/
int verifySameAssigment(int sameAssigments, int min, int max) {
    int count = 0;
    if (sameAssigments < min)
        count = 1;
    if (sameAssigments > max)
        count = 1;
    return count;
}

/** 
 * min = c->consecutive_working_shifts[0]
 * max = c->consecutive_working_shifts[1]
 **/
int verifyConsecutiveWorking(int consecutiveWorking, int min, int max) {
    int count = 0;
    if (consecutiveWorking < min)
        count = 1;
    if (consecutiveWorking > max)
        count = 1;
    return count;
}

/** 
 * min = c->number_of_assigments[0]
 * max = c->number_of_assigments[1]
 **/
//int verifyNumberOfAssigment(int numberAssingment, int min, int max) {
//    int count = 0;
//    if (numberAssingment < min)
//        count = 1;
//    if (numberAssingment > max)
//        count = 1;
//    return count;
//}

/** 
 * min = c->consecutive_assigments_matrix[shift][0]
 * max = c->consecutive_assigments_matrix[shift][1]
 **/
int verifyConsecutiveWorkingPerShift(int consecutiveWorking, int min, int max) {
    int count = 0;
    if (consecutiveWorking < min)
        count = 1;
    if (consecutiveWorking > max)
        count = 1;
    return count;
}

int consecutiveWorkShift_(int daySchadule, int dayProxSchadule, int _nurse, int* numberOfConsecutiveWork) {
    if (daySchadule == MORNING && dayProxSchadule == EVENING)
        numberOfConsecutiveWork[_nurse]++;
    else if (daySchadule == EVENING && dayProxSchadule == NIGHT)
        numberOfConsecutiveWork[_nurse]++;
    else if (daySchadule == NIGHT && dayProxSchadule == MORNING)
        numberOfConsecutiveWork[_nurse]++;
    return 0;
}

void constraints_cost_matrix(NspLib* nsp, Constraints* c, int day, int nurse, int** cost,
        int** assigment, List** dayPerNurse, int** numberOfConsecutiveWorking, int* numberOfFrees,
        int** numberOfSameAssigment, int* numberOfConsecutiveWork) {

    if (day > 0) {

        int scheduleDay[n_nurses];
        // calcular as quebras as restrições.

        for (int nurse2 = 0; nurse2 < n_nurses; nurse2++) {
            int nHCV = 0, nSCV = 0;

            scheduleDay[nurse] = getElementByIndex(dayPerNurse[day - 1], nurse);

            //Turnos seguidos que sao proibidos
            nHCV += verifyShift(assigment[nurse][nurse2], scheduleDay[nurse]);

            if (assigment[nurse][nurse2] == scheduleDay[nurse])
                numberOfSameAssigment[nurse2][scheduleDay[nurse]]++;

            int count = 0;
            for (int shift = 0; shift < n_shifts; shift++) {
                count += verifySameAssigment(numberOfSameAssigment[nurse][shift],
                        c->consecutive_assigments_matrix[shift][2], c->consecutive_assigments_matrix[shift][3]);
            }
            nSCV += count;

            consecutiveWorkShift_(scheduleDay[nurse], assigment[nurse][nurse2], nurse, numberOfConsecutiveWork);

            nSCV += verifyConsecutiveWorking(numberOfConsecutiveWork[nurse],
                    c->consecutive_working_shifts[0], c->consecutive_working_shifts[1]);

            if (assigment[nurse][nurse2] == scheduleDay[nurse]) {
                numberOfConsecutiveWorking[nurse][assigment[nurse][nurse2]] = 2;
            }

            if (assigment[nurse][nurse2] != scheduleDay[nurse]) {
                numberOfConsecutiveWorking[nurse][assigment[nurse][nurse2]] = 1;
                numberOfConsecutiveWorking[nurse][scheduleDay[nurse]] = 1;
            }

            if (day > (n_days / 2)) {
                if (numberOfFrees[nurse] > 0 && assigment[nurse][nurse2] == FREE) {
                    nHCV = 0;
                    nSCV = 0;
                }
            }

            count = 0;
            for (int shift = 0; shift < n_shifts; shift++) {
                count += verifyConsecutiveWorkingPerShift(numberOfConsecutiveWorking[nurse][shift],
                        c->consecutive_assigments_matrix[shift][0], c->consecutive_assigments_matrix[shift][1]);
            }
            nSCV += count;

            //Preenche Matrix de custos com base nos calculos
            cost[nurse][nurse2] =
                    nsp->preference_matrix[nurse][(day * n_shifts) + assigment[nurse][nurse2]] + Ph * nHCV + Ps * nSCV;

        }
    }
}

// =====================================================================================================================================================================
// ===                                                                                                                                                               ===     
// ===                                                                                                                                                               ===     
// ===                                                             CONSTRUÇÃO DA MATRIZ                                                                              ===     
// ===                                                               Fase Construtiva                                                                                ===     
// ===                                                                                                                                                               ===           
// ===                                                                                                                                                               ===           
// =====================================================================================================================================================================

int numberFreeShift(int min, int max) {
    return ((n_days - ((min + max) / 2)) * n_nurses) / n_days;
}

int numberFreeShiftPerNurse(int min, int max) {
    return (n_days - ((min + max) / 2));
}

void assingNurseToShift(int* vet, int shift) {
    if (shift == MORNING)
        vet[MORNING] = vet[MORNING] + 1;
    else if (shift == EVENING)
        vet[EVENING] = vet[EVENING] + 1;
    else if (shift == NIGHT)
        vet[NIGHT] = vet[NIGHT] + 1;
    else
        if (shift == FREE)
        vet[FREE] = vet[FREE] + 1;
}

int verificaRestricao(int shift, int previousShift) {
    if (previousShift == EVENING && shift == MORNING)
        return 0;
    if (previousShift == NIGHT && shift == MORNING)
        return 0;
    if (previousShift == NIGHT && shift == EVENING)
        return 0;
    else
        return 1;
}

void setMinimumCoverage(int* turnosNurse, int day, int nurse, int** coverage_matrix,
        int** preference_matrix, int** cost, int** assigment, int freeShifts, int* numberOfNursesPerShift) {
    int minimum_coverage;
    for (int j = 0; j < n_shifts; j++) {// shifts
        //verificar a matrix de cobertura e colocar a demanda minima.
        minimum_coverage = coverage_matrix[day][j];
        int k = 0;
        while (k < minimum_coverage) {
            cost[nurse][*turnosNurse] = preference_matrix[nurse][(day * n_shifts) + j];
            assigment[nurse][*turnosNurse] = j;
            assingNurseToShift(numberOfNursesPerShift, j); // atribui um enfermeiro a um turno
            if (j == FREE) {
                freeShifts--;
            }
            (*turnosNurse)++;
            k++;
        }
    }
}

void assignmentFreeShifts(int* turnos_nurse, int day, int nurse, int** preference_matrix,
        int** cost, int** assigment, int freeShifts, int* numberOfNursesPerShift, int* numberOfFrees) {
    if (numberOfFrees[nurse] > 0) {
        int f = 0;
        while (f <= freeShifts && *turnos_nurse < n_nurses) {
            cost[nurse][*turnos_nurse] = preference_matrix[nurse][(day * n_shifts) + FREE];
            assigment[nurse][*turnos_nurse] = FREE;
            assingNurseToShift(numberOfNursesPerShift, FREE); // atribui um enfermeiro a um turno
            (*turnos_nurse)++;
            f++;
        }
    }
}

void assignmentMinShiftsFirstDay(int* turnos_nurse, int day, int nurse, int** preference_matrix,
        int** cost, int** assigment, int* numberOfNursesPerShift, int* numberOfFrees) {
    int indice = *turnos_nurse;
    int max = (n_nurses - indice) + 1;
    int minShift[max];
    int minAtt[max];
    int h = 0;
    while (*turnos_nurse < n_nurses) {
        minShift[h] = INF;
        //Verifica o turno de menor custo
        for (int j = 0; j < n_shifts; j++) {// shifts
            if (numberOfFrees[nurse] <= 0) {
                if (j == FREE) {
                    break;
                }
            }
            cost[nurse][indice] = preference_matrix[nurse][(day * n_shifts) + j];
            assigment[nurse][indice] = j;
            if ((cost[nurse][indice]) < minShift[h]) {
                minShift[h] = cost[nurse][indice];
                minAtt[h] = assigment[nurse][indice];
            }
        }
        if (day >= ((n_days - 1) / 2)) {
            if (numberOfFrees[nurse] > 0) {
                minAtt[h] = FREE;
            }
        }
        cost[nurse][indice] = preference_matrix[nurse][(day * n_shifts) + minAtt[h]];
        assigment[nurse][indice] = minAtt[h];
        assingNurseToShift(numberOfNursesPerShift, minAtt[h]); // atribui um enfermeiro a um turno
        (*turnos_nurse)++;
        indice++;
        h++;
    }
}

void assignmentMinShifts(int* turnos_nurse, int day, int nurse, int** preference_matrix,
        int previousShift, int** cost, int** assigment, int* numberOfNursesPerShift, int* numberOfFrees) {
    int indice = *turnos_nurse;
    int max = (n_nurses - indice) + 1;
    int minShift[max];
    int minAtt[max];
    int h = 0;
    while (*turnos_nurse < n_nurses) {
        minShift[h] = INF;
        //Verifica o turno de menor custo
        for (int j = 0; j < n_shifts; j++) {// shifts
            if (numberOfFrees[nurse] <= 0) {
                if (j == FREE) {
                    break;
                }
            }
            cost[nurse][indice] = preference_matrix[nurse][(day * n_shifts) + j];
            assigment[nurse][indice] = j;
            if (verificaRestricao(j, previousShift)) {
                if ((cost[nurse][indice]) < minShift[h]) {
                    minShift[h] = cost[nurse][indice];
                    minAtt[h] = assigment[nurse][indice];
                }
            }
        }
        if (numberOfFrees[nurse] > 0) {
            minAtt[h] = FREE;
        }
        cost[nurse][indice] = preference_matrix[nurse][(day * n_shifts) + minAtt[h]];
        assigment[nurse][indice] = minAtt[h];
        assingNurseToShift(numberOfNursesPerShift, minAtt[h]); // atribui um enfermeiro a um turno
        (*turnos_nurse)++;
        indice++;
        h++;
    }
}

int combineScheduleHungarian(int day, NspLib* nsp, int** assigment, List** dayPerNurse,
        List** nursePerDay, hungarian_problem_t* hungarian, int* numberOfFrees) {
    int tcost = 0;

    for (int i = 0; i < n_nurses; i++) {

        int j = hungarian->assignment_vector[i];
        tcost += nsp->preference_matrix[i][(day * n_shifts) + assigment[i][j]];

        if (assigment[i][j] == FREE) {
            numberOfFrees[i]--;
        }

        int element = assigment[i][j];
        addLastList(nursePerDay[i], element);
        addLastList(dayPerNurse[day], element);
    }
    return tcost;
}

int combineScheduleApc(int day, NspLib* nsp, int** assigment, List** dayPerNurse,
        List** nursePerDay, apc_problem_t* apc, int* numberOfFrees) {
    int tcost = 0;

    for (int i = 0; i < n_nurses; i++) {

        int j = apc->colunm_assignment[i];
        tcost += nsp->preference_matrix[i][(day * n_shifts) + assigment[i][j]];

        if (assigment[i][j] == FREE) {
            numberOfFrees[i]--;
        }

        int element = assigment[i][j];
        addLastList(nursePerDay[i], element);
        addLastList(dayPerNurse[day], element);
    }
    return tcost;
}

int executeProceduteMultAssigmentHungarian(int d, NspLib* nsp, int** cost, int _cost,
        int** assigment, List** dayPerNurse, List** nursePerDay, int* numberOfFrees) {

    hungarian_problem_t* hungarian = (hungarian_problem_t*) calloc(1, sizeof (hungarian_problem_t));
    hungarian_init(hungarian, cost, n_nurses, n_nurses, HUNGARIAN_MODE_MINIMIZE_COST);
    hungarian_solve(hungarian);

    int c = combineScheduleHungarian(d, nsp, assigment, dayPerNurse, nursePerDay, hungarian, numberOfFrees);
    _cost += c;

    hungarian_free(hungarian);
    return _cost;
}

int executeProceduteMultAssigmentApc(int d, NspLib* nsp, int** cost, int _cost,
        int** assigment, List** dayPerNurse, List** nursePerDay, int* numberOfFrees) {
    int cost_optmal_assignment = 0, number_solutions = 0;

    apc_problem_t* vApc = (apc_problem_t*) calloc(1, sizeof (apc_problem_t));
    apc_init(vApc, n_nurses, cost, cost_optmal_assignment, number_solutions);
    apc(vApc);

    int c = combineScheduleApc(d, nsp, assigment, dayPerNurse, nursePerDay, vApc, numberOfFrees);
    _cost += c;

    apc_free(vApc);
    return _cost;
}

Schedule* buildFirstCostMatrix(NspLib* nsp, Constraints* c) {
    Schedule* s = (Schedule*) calloc(1, sizeof (Schedule));

    int _cost = 0;
    int** cost = (int**) calloc(n_nurses, sizeof (int*));
    int** assigment = (int**) calloc(n_nurses, sizeof (int*));
    List** dayPerNurse = (List**) calloc(n_days, sizeof (List*));
    List** nursePerDay = (List**) calloc(n_nurses, sizeof (List*));

    int* numberOfFrees = (int*) calloc(n_nurses, sizeof (int));
    int* numberOfAssigments = (int*) calloc(n_nurses, sizeof (int));
    int* numberOfNursesPerShift = (int*) calloc(n_nurses, sizeof (int));
    int* numberOfConsecutiveWork = (int*) calloc(n_nurses, sizeof (int));
    int* maxNumberOfConsecutiveWork = (int*) calloc(n_nurses, sizeof (int));

    int** numberOfSameAssigment = (int**) calloc(n_nurses, sizeof (int*));
    int** numberOfConsecutiveWorking = (int**) calloc(n_nurses, sizeof (int*));
    int** maxNumberOfConsecutiveWorking = (int**) calloc(n_nurses, sizeof (int*));

    for (int n = 0; n < n_nurses; n++) {
        numberOfAssigments[n] = 0;
        numberOfNursesPerShift[n] = 0;
        numberOfConsecutiveWork[n] = 0;

        cost[n] = (int*) calloc(n_nurses, sizeof (int));
        assigment[n] = (int*) calloc(n_nurses, sizeof (int));

        numberOfSameAssigment[n] = (int*) calloc(n_shifts, sizeof (int));
        numberOfConsecutiveWorking[n] = (int*) calloc(n_shifts, sizeof (int));
        maxNumberOfConsecutiveWorking[n] = (int*) calloc(n_shifts, sizeof (int));

        for (int j = 0; j < n_shifts; j++) {
            numberOfSameAssigment[n][j] = 0;
            numberOfConsecutiveWorking[n][j] = 1;
            maxNumberOfConsecutiveWorking[n][j] = 1;
        }
        for (int j = 0; j < n_nurses; j++) {
            assigment[n][j] = 0;
        }
        numberOfFrees[n] = numberFreeShiftPerNurse(c->number_of_assigments[0], c->number_of_assigments[1]);
    }

    int freeShifts = numberFreeShift(c->number_of_assigments[0], c->number_of_assigments[1]);
    for (int d = 0; d < n_days; d++) {//days

        int scheduleDay[n_nurses];
        dayPerNurse[d] = initializeList();

        for (int n = 0; n < n_nurses; n++) {
            if (d == 0)
                nursePerDay[n] = initializeList();

            if (d > 0)
                scheduleDay[n] = getElementByIndex(dayPerNurse[d - 1], n);

            int turnosNurses = 0;
            //verificar a matrix de cobertura e colocar a demanda minima.
            setMinimumCoverage(&turnosNurses, d, n, nsp->coverage_matrix, nsp->preference_matrix,
                    cost, assigment, freeShifts, numberOfNursesPerShift);

            // colocar quantidade de turno frees
            assignmentFreeShifts((&turnosNurses), d, n, nsp->preference_matrix, cost, assigment,
                    freeShifts, numberOfNursesPerShift, numberOfFrees);

            //completar com o turno de menor custo o que sobrar sem nada alocado
            if (d == 0)
                assignmentMinShiftsFirstDay((&turnosNurses), d, n, nsp->preference_matrix, cost,
                    assigment, numberOfNursesPerShift, numberOfFrees);

            if (d > 0)
                assignmentMinShifts(&turnosNurses, d, n, nsp->preference_matrix, scheduleDay[n],
                    cost, assigment, numberOfNursesPerShift, numberOfFrees);

            // calcular as quebras as restrições.
            constraints_cost_matrix(nsp, c, d, n, cost, assigment, dayPerNurse,
                    numberOfConsecutiveWorking, numberOfFrees, numberOfSameAssigment, numberOfConsecutiveWork);
        }
        if (useApcMultSolution)
            _cost = executeProceduteMultAssigmentApc(d, nsp, cost, _cost, assigment,
                dayPerNurse, nursePerDay, numberOfFrees);
        if (useHungarianSolution)
            _cost = executeProceduteMultAssigmentHungarian(d, nsp, cost, _cost, assigment,
                dayPerNurse, nursePerDay, numberOfFrees);
    }

    s->day_per_nurse = (List**) calloc(n_days, sizeof (List*));
    s->nurse_per_day = (List**) calloc(n_nurses, sizeof (List*));
    for (int d = 0; d < n_days; d++) {
        s->day_per_nurse[d] = initializeList();
        s->day_per_nurse[d] = copyList(dayPerNurse[d], s->day_per_nurse[d]);
    }
    for (int n = 0; n < n_nurses; n++) {
        s->nurse_per_day[n] = initializeList();
        s->nurse_per_day[n] = copyList(nursePerDay[n], s->nurse_per_day[n]);
    }
    s->cost_solution = _cost;

    free(numberOfAssigments);
    free(numberOfNursesPerShift);
    free(numberOfConsecutiveWork);
    free(maxNumberOfConsecutiveWork);
    for (int j = 0; j < n_shifts; j++) {
        free(numberOfSameAssigment[j]);
        free(numberOfConsecutiveWorking[j]);
        free(maxNumberOfConsecutiveWorking[j]);
    }
    free(numberOfSameAssigment);
    free(numberOfConsecutiveWorking);
    free(maxNumberOfConsecutiveWorking);

    for (int i = 0; i < n_nurses; i++) {
        free(assigment[i]);
        free(cost[i]);
    }
    free(assigment);
    free(cost);
    for (int i = 0; i < n_nurses; i++) {
        freeList(nursePerDay[i]);
    }
    for (int i = 0; i < n_days; i++) {
        freeList(dayPerNurse[i]);
    }
    dayPerNurse = NULL;
    nursePerDay = NULL;
    return s;
}

// =====================================================================================================================================================================
// ===                                                                                                                                                               ===
// ===                                                                                                                                                               ===
// ===                                                             Construção dos custos                                                                             ===
// ===                                                                   Operadores                                                                                  ===
// ===                                                                                                                                                               ===
// ===                                                                                                                                                               ===
// =====================================================================================================================================================================

int constrants_cost_matrix_com_escala(int *_cost, int *hardConstraints, int *softConstraints,
                                      Schedule *s, NspLib *nsp, Constraints *c, Node *nurse, int days,
                                      int **minimum_coverage) {
    int index_n = 0, cost = 0;
    while (nurse != NULL) {
        int nHCV = 0, nSCV = 0;
        int vet = 1;
        if (days == 0) {
            nHCV = sequencial_shifts(s->nurse_per_day[index_n], &vet);

            if (verify_number_of_assigments(vet, c->number_of_assigments) == 1) {
                nHCV++;
            }
        }
        int *assignments_shift = shifts_per_day_vector(s->nurse_per_day, index_n);
        for (int a = 0; a < n_shifts; a++) {
            if (assignments_shift[a] > c->consecutive_assigments_matrix[3][a]
            && assignments_shift[a] < c->consecutive_assigments_matrix[2][a]) {
                nSCV++;
            }
        }
        int consecutive_work_shifts = consecutive_work_shift(s->nurse_per_day, index_n);
        if (consecutive_work_shifts > c->consecutive_working_shifts[1] &&
        consecutive_work_shifts < c->consecutive_working_shifts[0]) {
            nHCV++;
        }
        int *consecutive_same_work_shifts = consecutive_same_work_shift(s->nurse_per_day, index_n);
        for (int a = 0; a < n_shifts; a++) {
            if (consecutive_same_work_shifts[a] > c->consecutive_assigments_matrix[a][1]
            && consecutive_same_work_shifts[a] < c->consecutive_assigments_matrix[a][0]) {
                nSCV++;
            }
        }

        if (verify_minimum_coverage(days, nsp->coverage_matrix, nurse->data, minimum_coverage[days][nurse->data])) {
            nSCV++;
        }

        cost += nsp->preference_matrix[index_n][(days * n_shifts) + nurse->data] + Ph * nHCV + Ps * nSCV;
        (*hardConstraints) += nHCV;
        (*softConstraints) += nSCV;
        index_n++;
        nurse = nurse->next;
    }
    (*_cost) += cost;
    return cost;
}

int verify_constraints(Constraints*c, Schedule* sc, int n) {
    int nHCV = 0;
    int vet = 1;
    nHCV = sequencial_shifts(sc->nurse_per_day[n], &vet);

    if (verify_number_of_assigments(vet, c->number_of_assigments) == 1) {
        nHCV++;
    }
    return (Ph * nHCV);
}