#include "operators.h"

Schedule* operatorKswap(Schedule* s, NspLib* nsp, Constraints* c, int k);

int **mshifts_per_day(List **day_per_nurse) {
    int **rt = (int **) calloc(n_days, sizeof(int *));

    for (int d = 0; d < n_days; d++) {
        rt[d] = (int *) calloc(n_shifts, sizeof(int));
        List *nurses = day_per_nurse[d];
        Node *e = nurses->first;

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

hungarian_problem_t *useHungarian(int d, Schedule *s, int **m_cost2, int **m_assigment, hungarian_problem_t *p) {
    hungarian_init(p, m_cost2, n_nurses, n_nurses, HUNGARIAN_MODE_MINIMIZE_COST);
    hungarian_solve(p);
    int cost;
    for (int i = 0; i < n_nurses; i++) {
        cost += m_assigment[i][p->assignment_vector[i]];
    }
    return p;
}

void recombine_schedule_kswap_day(int d, Schedule* s) {
    for (int day = 0; day < n_days; day++) {
        for (int i = 0; i < n_nurses; i++) {
            List* list1 = s->day_per_nurse[day];
            Node* n1 = getNodeByIndex(list1, i);
            List* list2 = s->nurse_per_day[i];
            Node* n2 = getNodeByIndex(list2, day);
            n1->data = n2->data;
        }
    }
}

void recombine_schedule_kswp_nurse(int K, int day, int* assignment_vector, Schedule* s) {
    for (int i = 0; i < n_nurses; i++) {
        List* list1 = s->nurse_per_day[i];
        List** list2 = (List**) calloc(n_nurses, sizeof (List*));
        for (int j = 0; j < n_nurses; j++) {
            list2[j] = initializeList();
            list2[j] = copyList(s->nurse_per_day[j], list2[j]);
        }
        for (int d = day; d < (day + 1) + K; d++) {
            Node* n2 = getNodeByIndex(list2[assignment_vector[i]], d);
            Node* n1 = getNodeByIndex(list1, d);
            n1->data = n2->data;
        }
    }
}

void recombine_schedule_kSwap(int* assignment_vector, Schedule* s, int day, int k) {
    recombine_schedule_kswp_nurse(k, day, assignment_vector, s);
    recombine_schedule_kswap_day(day, s);
}

void recombine_schedule_pcr_nurse(int day, int *assignment_vector, Schedule *s) {
    for (int i = 0; i < n_nurses; i++) {
        List *list1 = s->nurse_per_day[i];
        List **list2 = (List **) calloc(n_nurses, sizeof(List *));
        for (int j = 0; j < n_nurses; j++) {
            list2[j] = initializeList();
            list2[j] = copyList(s->nurse_per_day[j], list2[j]);
        }
        for (int d = day; d < n_days; d++) {
            Node *n2 = getNodeByIndex(list2[assignment_vector[i]], d);
            Node *n1 = getNodeByIndex(list1, d);
            n1->data = n2->data;
        }
    }
}

void recombine_schedule_pcr_day(int d, Schedule *s) {
    for (int day = d; day < n_days; day++) {
        for (int i = 0; i < n_nurses; i++) {
            List *list1 = s->day_per_nurse[day];
            Node *n1 = getNodeByIndex(list1, i);
            List *list2 = s->nurse_per_day[i];
            Node *n2 = getNodeByIndex(list2, day);
            n1->data = n2->data;
        }
    }
}

void recombine_schedule_pcr(int *assignment_vector, Schedule *s, int day) {
    if (day > 0) {
        recombine_schedule_pcr_nurse(day, assignment_vector, s);
        recombine_schedule_pcr_day(day, s);
    }
}

void recombine_schedule(int **m_assigment, int *assignment_vector, Schedule *s, int day) {
    if (day > 0) {
        for (int i = 0; i < n_nurses; i++) {
            setList(s->day_per_nurse[day], m_assigment[i][assignment_vector[i]], i);
        }
        for (int i = 0; i < n_nurses; i++) {
            setList(s->nurse_per_day[i], m_assigment[i][assignment_vector[i]], day);
        }
    }
}

Schedule* kSwap(Schedule* s, NspLib* nsp, Constraints* c, int k) {
    if (k == 1) {
        s = prt(s, nsp, c);
    } else {
        s = operatorKswap(s, nsp, c, k);
    }
    return s;
}


Schedule *pcr(Schedule *s, NspLib *nsp, Constraints *c) {
    for (int day = 0; day < n_days - 1; day++) {
        int **m_cost = (int **) calloc(n_nurses, sizeof(int *));
        int **m_assigment = (int **) calloc(n_nurses, sizeof(int *));
        int d = day + 1;
        for (int nurse1 = 0; nurse1 < n_nurses; nurse1++) {
            m_cost[nurse1] = (int *) calloc(n_nurses, sizeof(int));
            m_assigment[nurse1] = (int *) calloc(n_nurses, sizeof(int));

            List *list1 = s->nurse_per_day[nurse1];
            Node *n1 = getNodeByIndex(list1, day);
            Node *aux = n1->next;
            for (int nurse2 = 0; nurse2 < n_nurses; nurse2++) {
                List *list2 = s->nurse_per_day[nurse2];
                Node *n2 = getNodeByIndex(list2, d);
                int aux2 = n2->data;

                n1->next = n2;
                //calculos
                int **minimum_coverage = mshifts_per_day(s->day_per_nurse);
                int cost = 0, hcs = 0, sfc = 0;
                constrants_cost_matrix_com_escala(&cost, &hcs, &sfc, s, nsp, c, n2, day, minimum_coverage);
                m_cost[nurse1][nurse2] += cost;
                m_assigment[nurse1][nurse2] = aux2;

                n1->next = aux;
            }
        }
        //hungaro
        //hungarian algorithm
        hungarian_problem_t *p = (hungarian_problem_t *) calloc(1, sizeof(hungarian_problem_t));
        p = useHungarian(d, s, m_cost, m_assigment, p);
        recombine_schedule_pcr(p->assignment_vector, s, d);
        hungarian_free(p);

        for (int i = 0; i < n_nurses; i++) {
            free(m_cost[i]);
            free(m_assigment[i]);
        }
        free(m_cost);
        free(m_assigment);
    }
    return s;
}

Schedule *prt(Schedule *s, NspLib *nsp, Constraints *c) {
    for (int d = 0; d < n_days; d++) {
        int **m_cost = (int **) calloc(n_nurses, sizeof(int *));
        int **m_assigment = (int **) calloc(n_nurses, sizeof(int *));
        for (int nurse1 = 0; nurse1 < n_nurses; nurse1++) {
            m_cost[nurse1] = (int *) calloc(n_nurses, sizeof(int));
            m_assigment[nurse1] = (int *) calloc(n_nurses, sizeof(int));

            List *list1 = s->nurse_per_day[nurse1];
            Node *n1;
            Node *p_n1;
            Node *p_aux;
            if (d < n_days - 1)
                n1 = getNodeByIndex(list1, d + 1);
            if (d > 0) {
                p_n1 = getNodeByIndex(list1, d - 1);
                p_aux = p_n1->next;
            }

            for (int nurse2 = 0; nurse2 < n_nurses; nurse2++) {
                List *list2 = s->nurse_per_day[nurse2];
                Node *aux;
                Node *n2 = getNodeByIndex(list2, d);

                if (d < n_days - 1) {
                    aux = n2->next;
                    n2->next = n1;
                }
                if (d > 0)
                    p_n1->next = n2;

                //calcula custos
                int cons = 0;
                if (d == 0) {
                    cons = verify_constraints(c, s, nurse2);
                } else {
                    cons = verify_constraints(c, s, nurse1);
                }
                m_cost[nurse1][nurse2] += nsp->preference_matrix[nurse1][(d * n_shifts) + n2->data] + cons;
                m_assigment[nurse1][nurse2] = n2->data;
                //                }

                if (d > 0)
                    p_n1->next = p_aux;
                if (d < n_days - 1)
                    n2->next = aux;
            }
        }

        //hungaro
        //hungarian algorithm
        hungarian_problem_t *p = (hungarian_problem_t *) calloc(1, sizeof(hungarian_problem_t));
        p = useHungarian(d, s, m_cost, m_assigment, p);
        recombine_schedule(m_assigment, p->assignment_vector, s, d);
        hungarian_free(p);

        for (int i = 0; i < n_nurses; i++) {
            free(m_cost[i]);
            free(m_assigment[i]);
        }
        free(m_cost);
        free(m_assigment);
    }
    return s;
}

Schedule* operatorKswap(Schedule* s, NspLib* nsp, Constraints* c, int k) {
    for (int d = 0; d < (n_days - k); d++) {
        int dcut = d + k;
        int** m_cost = (int**) calloc(n_nurses, sizeof (int**));
        int** m_assigment = (int**) calloc(n_nurses, sizeof (int**));

        for (int nurse1 = 0; nurse1 < n_nurses; nurse1++) {
            m_cost[nurse1] = (int*) calloc(n_nurses, sizeof (int*));
            m_assigment[nurse1] = (int*) calloc(n_nurses, sizeof (int*));

            List* list1 = s->nurse_per_day[nurse1];
            Node* n1;
            Node* p_n1;
            Node* p_aux;
            if (dcut < n_days - 1)
                n1 = getNodeByIndex(list1, dcut + 1);
            if (d > 0) {
                p_n1 = getNodeByIndex(list1, d - 1);
                p_aux = p_n1->next;
            }
            for (int nurse2 = 0; nurse2 < n_nurses; nurse2++) {
                int data = 0;
                List* list2 = s->nurse_per_day[nurse2];
                Node* aux;
                Node* p_n2 = getNodeByIndex(list2, d);
                Node* n2 = getNodeByIndex(list2, dcut);
                if (dcut < n_days - 1) {
                    aux = n2->next;
                    n2->next = n1;
                }
                if (d > 0)
                    p_n1->next = p_n2;

                //calcula custos
                if (d == 0)
                    data = n2->data;
                data = p_n2->data;
                int** minimum_coverage = mshifts_per_day(s->day_per_nurse);
                int cost = 0, hcs = 0, sfc = 0;
                constrants_cost_matrix_com_escala(&cost, &hcs, &sfc, s, nsp, c, n2, d, minimum_coverage);
                m_cost[nurse1][nurse2] += cost;
                m_assigment[nurse1][nurse2] = data;

                if (d > 0)
                    p_n1->next = p_aux;
                if (dcut < n_days - 1)
                    n2->next = aux;
            }
        }

            //hungaro
            //hungarian algorithm
            hungarian_problem_t *p = (hungarian_problem_t*) calloc(1, sizeof (hungarian_problem_t));
            p = useHungarian(dcut, s, m_cost, m_assigment, p);
            recombine_schedule_kSwap(p->assignment_vector, s, d, k);
            hungarian_free(p);


        for (int i = 0; i < n_nurses; i++) {
            free(m_cost[i]);
            free(m_assigment[i]);
        }
        free(m_cost);
        free(m_assigment);
    }
    return s;
}

Schedule *pcr_backward(Schedule *s, NspLib *nsp, Constraints *constraintsLocalPcrBackward) {
    for (int i = 0; i < n_nurses; i++)
        invertList(s->nurse_per_day[i]);

    s = pcr(s, nsp, constraintsLocalPcrBackward);

    for (int i = 0; i < n_nurses; i++)
        invertList(s->nurse_per_day[i]);

    for (int i = 0; i < n_days; i++)
        invertList(s->day_per_nurse[i]);

    return s;
}

Schedule *prt_backward(Schedule *s, NspLib *nsp, Constraints *constraintsLocalPrtBackward) {
    for (int i = 0; i < n_nurses; i++)
        invertList(s->nurse_per_day[i]);

    s = prt(s, nsp, constraintsLocalPrtBackward);

    for (int i = 0; i < n_nurses; i++)
        invertList(s->nurse_per_day[i]);

    for (int i = 0; i < n_days; i++)
        invertList(s->day_per_nurse[i]);

    return s;
}

Schedule * kSwap_backward(Schedule* s, NspLib* nsp, Constraints * c, int k) {
    for (int i = 0; i < n_nurses; i++)
        invertList(s->nurse_per_day[i]);

    s = kSwap(s, nsp, c, k);

    for (int i = 0; i < n_nurses; i++)
        invertList(s->nurse_per_day[i]);

    for (int i = 0; i < n_days; i++)
        invertList(s->day_per_nurse[i]);

    return s;
}
