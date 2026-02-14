#include "kswap.h"
#include "../core/cost.h"
#include "../core/schedule.h"

static void kswap_prt(Schedule *s, const NspInstance *nsp,
                      const Constraints *c, LapSolverFn solver) {
    int n = s->size.n_nurses;
    int d = s->size.n_days;
    int ns = s->size.n_shifts;

    int **cost_m = alloc_matrix_int(n, n);
    int *result = (int *)calloc((size_t)n, sizeof(int));

    for (int day = 0; day < d; day++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                int sh = s->nurse_schedule[j][day];
                int pref = nsp->preference_matrix[i][day * ns + sh];
                int penalty = 0;

                if (day > 0 && is_forbidden_sequence(s->nurse_schedule[i][day - 1], sh))
                    penalty += PENALTY_HARD;
                if (day < d - 1 && is_forbidden_sequence(sh, s->nurse_schedule[i][day + 1]))
                    penalty += PENALTY_HARD;

                cost_m[i][j] = pref + penalty;
            }
        }

        solver(n, cost_m, result);

        int *new_day = (int *)calloc((size_t)n, sizeof(int));
        for (int i = 0; i < n; i++)
            new_day[i] = s->nurse_schedule[result[i]][day];
        for (int i = 0; i < n; i++)
            s->nurse_schedule[i][day] = new_day[i];
        free(new_day);
    }

    free_matrix_int(cost_m, n);
    free(result);
}

void kswap_forward(Schedule *s, const NspInstance *nsp,
                   const Constraints *c, LapSolverFn solver, int k) {
    int n = s->size.n_nurses;
    int d = s->size.n_days;

    if (k == 1) {
        kswap_prt(s, nsp, c, solver);
        cost_evaluate_silent(s, c, nsp);
        return;
    }

    int **cost_m = alloc_matrix_int(n, n);
    int *result = (int *)calloc((size_t)n, sizeof(int));

    for (int day = 0; day <= d - k; day++) {
        int day_end = day + k - 1;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                Schedule *temp = schedule_copy(s);

                for (int dd = day; dd <= day_end; dd++)
                    temp->nurse_schedule[i][dd] = s->nurse_schedule[j][dd];

                cost_m[i][j] = cost_partial_nurse(temp, c, nsp, i);
                schedule_free(temp);
            }
        }

        solver(n, cost_m, result);

        int **new_block = alloc_matrix_int(n, k);
        for (int i = 0; i < n; i++)
            for (int dd = 0; dd < k; dd++)
                new_block[i][dd] = s->nurse_schedule[result[i]][day + dd];

        for (int i = 0; i < n; i++)
            for (int dd = 0; dd < k; dd++)
                s->nurse_schedule[i][day + dd] = new_block[i][dd];

        free_matrix_int(new_block, n);
    }

    free_matrix_int(cost_m, n);
    free(result);

    cost_evaluate_silent(s, c, nsp);
}

void kswap_backward(Schedule *s, const NspInstance *nsp,
                    const Constraints *c, LapSolverFn solver, int k) {
    schedule_reverse_days(s);
    kswap_forward(s, nsp, c, solver, k);
    schedule_reverse_days(s);
    cost_evaluate_silent(s, c, nsp);
}
