#ifndef MAPA_VNS_H
#define MAPA_VNS_H

#include "../core/types.h"

typedef struct {
    int max_iterations;
    int max_no_improve;
    int k_max;
    bool use_bap;
} VnsConfig;

VnsConfig vns_default_config(void);

Schedule *vns_solve(Schedule *initial, const NspInstance *nsp,
                    const Constraints *c, LapSolverFn lap_solver,
                    LapSolverFn bap_solver, const VnsConfig *config);

#endif
