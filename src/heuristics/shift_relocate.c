#include "shift_relocate.h"
#include "../core/cost.h"
#include "../core/schedule.h"

void shift_relocate_forward(Schedule *s, const NspInstance *nsp,
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
            for (int day = 0; day < d; day++) {
                int orig_shift = s->nurse_schedule[nurse][day];
                if (orig_shift == SHIFT_FREE) continue;

                int best_target = -1;
                int best_target_day = -1;

                for (int target = 0; target < n; target++) {
                    if (target == nurse) continue;
                    if (s->nurse_schedule[target][day] != SHIFT_FREE) continue;

                    s->nurse_schedule[nurse][day] = SHIFT_FREE;
                    s->nurse_schedule[target][day] = orig_shift;

                    cost_evaluate_silent(s, c, nsp);
                    if (s->cost < best_cost) {
                        best_cost = s->cost;
                        best_target = target;
                        best_target_day = day;
                    }

                    s->nurse_schedule[nurse][day] = orig_shift;
                    s->nurse_schedule[target][day] = SHIFT_FREE;
                }

                if (best_target >= 0) {
                    s->nurse_schedule[nurse][best_target_day] = SHIFT_FREE;
                    s->nurse_schedule[best_target][best_target_day] = orig_shift;
                    cost_evaluate_silent(s, c, nsp);
                    improved = true;
                }
            }
        }
    }
}

void shift_relocate_backward(Schedule *s, const NspInstance *nsp,
                             const Constraints *c, LapSolverFn solver) {
    schedule_reverse_days(s);
    shift_relocate_forward(s, nsp, c, solver);
    schedule_reverse_days(s);
    cost_evaluate_silent(s, c, nsp);
}
