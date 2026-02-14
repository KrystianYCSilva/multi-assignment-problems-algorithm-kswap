#include "swap_exchange.h"
#include "../core/cost.h"
#include "../core/schedule.h"

void swap_exchange_forward(Schedule *s, const NspInstance *nsp,
                           const Constraints *c, LapSolverFn solver) {
    (void)solver;
    int n = s->size.n_nurses;
    int d = s->size.n_days;
    bool improved = true;

    while (improved) {
        improved = false;
        cost_evaluate_silent(s, c, nsp);
        int best_cost = s->cost;

        for (int day = 0; day < d; day++) {
            for (int i = 0; i < n - 1; i++) {
                for (int j = i + 1; j < n; j++) {
                    if (s->nurse_schedule[i][day] == s->nurse_schedule[j][day])
                        continue;

                    int tmp = s->nurse_schedule[i][day];
                    s->nurse_schedule[i][day] = s->nurse_schedule[j][day];
                    s->nurse_schedule[j][day] = tmp;

                    cost_evaluate_silent(s, c, nsp);
                    if (s->cost < best_cost) {
                        best_cost = s->cost;
                        improved = true;
                    } else {
                        s->nurse_schedule[j][day] = s->nurse_schedule[i][day];
                        s->nurse_schedule[i][day] = tmp;
                    }
                }
            }
        }
        cost_evaluate_silent(s, c, nsp);
    }
}

void swap_exchange_backward(Schedule *s, const NspInstance *nsp,
                            const Constraints *c, LapSolverFn solver) {
    schedule_reverse_days(s);
    swap_exchange_forward(s, nsp, c, solver);
    schedule_reverse_days(s);
    cost_evaluate_silent(s, c, nsp);
}
