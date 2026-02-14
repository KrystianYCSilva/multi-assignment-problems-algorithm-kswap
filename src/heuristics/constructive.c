#include "constructive.h"
#include "../core/cost.h"
#include "../core/schedule.h"

static int compute_free_per_nurse(const Constraints *c, int n_days) {
    int min_a = c->number_of_assignments[0];
    int max_a = c->number_of_assignments[1];
    int avg = (min_a + max_a) / 2;
    return n_days - avg;
}

Schedule *constructive_build(const NspInstance *nsp, const Constraints *c,
                             LapSolverFn solver) {
    int n = nsp->size.n_nurses;
    int d = nsp->size.n_days;
    int s = nsp->size.n_shifts;

    Schedule *sched = schedule_create(n, d);
    if (!sched) return NULL;

    int *free_remaining = (int *)calloc((size_t)n, sizeof(int));
    int free_per_nurse = compute_free_per_nurse(c, d);
    for (int i = 0; i < n; i++)
        free_remaining[i] = free_per_nurse;

    int **cost_m = alloc_matrix_int(n, n);
    int **assign_m = alloc_matrix_int(n, n);
    int *result = (int *)calloc((size_t)n, sizeof(int));

    for (int day = 0; day < d; day++) {
        for (int nurse = 0; nurse < n; nurse++) {
            int col = 0;
            for (int sh = 0; sh < NUM_WORK_SHIFTS; sh++) {
                int demand = nsp->coverage_matrix[day][sh];
                for (int k = 0; k < demand && col < n; k++) {
                    cost_m[nurse][col] = nsp->preference_matrix[nurse][day * s + sh];
                    assign_m[nurse][col] = sh;
                    if (day > 0 && is_forbidden_sequence(sched->nurse_schedule[nurse][day - 1], sh))
                        cost_m[nurse][col] += PENALTY_HARD;
                    col++;
                }
            }

            if (free_remaining[nurse] > 0) {
                while (col < n) {
                    cost_m[nurse][col] = nsp->preference_matrix[nurse][day * s + SHIFT_FREE];
                    assign_m[nurse][col] = SHIFT_FREE;
                    col++;
                }
            } else {
                int best_sh = SHIFT_MORNING;
                int best_cost = COST_INF;
                for (int sh = 0; sh < NUM_WORK_SHIFTS; sh++) {
                    int c_val = nsp->preference_matrix[nurse][day * s + sh];
                    if (day > 0 && is_forbidden_sequence(sched->nurse_schedule[nurse][day - 1], sh))
                        c_val += PENALTY_HARD;
                    if (c_val < best_cost) { best_cost = c_val; best_sh = sh; }
                }
                while (col < n) {
                    cost_m[nurse][col] = nsp->preference_matrix[nurse][day * s + best_sh];
                    assign_m[nurse][col] = best_sh;
                    if (day > 0 && is_forbidden_sequence(sched->nurse_schedule[nurse][day - 1], best_sh))
                        cost_m[nurse][col] += PENALTY_HARD;
                    col++;
                }
            }
        }

        solver(n, cost_m, result);

        for (int nurse = 0; nurse < n; nurse++) {
            int assigned_shift = assign_m[nurse][result[nurse]];
            sched->nurse_schedule[nurse][day] = assigned_shift;
            if (assigned_shift == SHIFT_FREE)
                free_remaining[nurse]--;
        }
    }

    free_matrix_int(cost_m, n);
    free_matrix_int(assign_m, n);
    free(result);
    free(free_remaining);

    return sched;
}
