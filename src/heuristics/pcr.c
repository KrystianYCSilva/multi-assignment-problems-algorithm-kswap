#include "pcr.h"
#include "../core/cost.h"
#include "../core/schedule.h"

void pcr_forward(Schedule *s, const NspInstance *nsp,
                 const Constraints *c, LapSolverFn solver) {
    int n = s->size.n_nurses;
    int d = s->size.n_days;

    int **cost_m = alloc_matrix_int(n, n);
    int *result = (int *)calloc((size_t)n, sizeof(int));

    for (int cut = 1; cut < d; cut++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                Schedule *temp = schedule_copy(s);

                for (int dd = cut; dd < d; dd++)
                    temp->nurse_schedule[i][dd] = s->nurse_schedule[j][dd];

                cost_m[i][j] = cost_partial_nurse(temp, c, nsp, i);
                schedule_free(temp);
            }
        }

        solver(n, cost_m, result);

        int **new_tail = alloc_matrix_int(n, d);
        for (int i = 0; i < n; i++)
            for (int dd = cut; dd < d; dd++)
                new_tail[i][dd] = s->nurse_schedule[result[i]][dd];

        for (int i = 0; i < n; i++)
            for (int dd = cut; dd < d; dd++)
                s->nurse_schedule[i][dd] = new_tail[i][dd];

        free_matrix_int(new_tail, n);
    }

    free_matrix_int(cost_m, n);
    free(result);

    cost_evaluate_silent(s, c, nsp);
}

void pcr_backward(Schedule *s, const NspInstance *nsp,
                  const Constraints *c, LapSolverFn solver) {
    schedule_reverse_days(s);
    pcr_forward(s, nsp, c, solver);
    schedule_reverse_days(s);
    cost_evaluate_silent(s, c, nsp);
}
