#include "two_opt.h"
#include "../core/cost.h"
#include "../core/schedule.h"
#include <string.h>

static void reverse_segment(int *arr, int start, int end) {
    while (start < end) {
        int tmp = arr[start];
        arr[start] = arr[end];
        arr[end] = tmp;
        start++;
        end--;
    }
}

void two_opt_forward(Schedule *s, const NspInstance *nsp,
                     const Constraints *c, LapSolverFn solver) {
    (void)solver;
    int n = s->size.n_nurses;
    int d = s->size.n_days;
    bool improved = true;

    while (improved) {
        improved = false;
        cost_evaluate_silent(s, c, nsp);
        int best_cost = s->cost;

        for (int nurse = 0; nurse < n; nurse++) {
            int *row = s->nurse_schedule[nurse];
            int *backup = (int *)calloc((size_t)d, sizeof(int));
            memcpy(backup, row, (size_t)d * sizeof(int));

            for (int i = 0; i < d - 1; i++) {
                for (int j = i + 1; j < d; j++) {
                    reverse_segment(row, i, j);

                    cost_evaluate_silent(s, c, nsp);
                    if (s->cost < best_cost) {
                        best_cost = s->cost;
                        memcpy(backup, row, (size_t)d * sizeof(int));
                        improved = true;
                    } else {
                        reverse_segment(row, i, j);
                    }
                }
            }
            memcpy(row, backup, (size_t)d * sizeof(int));
            free(backup);
        }
        cost_evaluate_silent(s, c, nsp);
    }
}

void two_opt_backward(Schedule *s, const NspInstance *nsp,
                      const Constraints *c, LapSolverFn solver) {
    schedule_reverse_days(s);
    two_opt_forward(s, nsp, c, solver);
    schedule_reverse_days(s);
    cost_evaluate_silent(s, c, nsp);
}
