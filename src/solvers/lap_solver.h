#ifndef MAPA_LAP_SOLVER_H
#define MAPA_LAP_SOLVER_H

#include "../core/types.h"

int lap_solve_hungarian(int n, int **cost, int *assignment);

int lap_solve_apc(int n, int **cost, int *assignment);

int bap_solve(int n, int **cost, int *assignment);

#endif
