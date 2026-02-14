#include "vns.h"
#include "pcr.h"
#include "kswap.h"
#include "../core/cost.h"
#include "../core/schedule.h"
#include <stdio.h>

VnsConfig vns_default_config(void) {
    VnsConfig cfg;
    cfg.max_iterations = 100;
    cfg.max_no_improve = 5;
    cfg.k_max = 4;
    cfg.use_bap = false;
    return cfg;
}

Schedule *vns_solve(Schedule *initial, const NspInstance *nsp,
                    const Constraints *c, LapSolverFn lap_solver,
                    LapSolverFn bap_solver_fn, const VnsConfig *config) {
    Schedule *best = schedule_copy(initial);
    cost_evaluate_silent(best, c, nsp);

    Schedule *current = schedule_copy(best);
    int no_improve = 0;

    for (int iter = 0; iter < config->max_iterations && no_improve < config->max_no_improve; iter++) {
        LapSolverFn solver = config->use_bap ? bap_solver_fn : lap_solver;

        Schedule *candidate = schedule_copy(current);

        pcr_forward(candidate, nsp, c, solver);
        kswap_forward(candidate, nsp, c, solver, 1);
        pcr_backward(candidate, nsp, c, solver);
        kswap_backward(candidate, nsp, c, solver, 1);

        for (int k = config->k_max; k >= 2; k--)
            kswap_forward(candidate, nsp, c, solver, k);

        cost_evaluate_silent(candidate, c, nsp);

        if (candidate->cost < current->cost) {
            schedule_free(current);
            current = candidate;
            no_improve = 0;

            if (current->cost < best->cost) {
                schedule_free(best);
                best = schedule_copy(current);
            }
        } else {
            schedule_free(candidate);
            no_improve++;
        }

        printf("VNS iter=%d cost=%d best=%d no_improve=%d\n",
               iter, current->cost, best->cost, no_improve);
    }

    schedule_free(current);
    return best;
}
