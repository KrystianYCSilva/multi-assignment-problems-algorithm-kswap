#ifndef MAPA_TWO_OPT_H
#define MAPA_TWO_OPT_H

#include "../core/types.h"

void two_opt_forward(Schedule *s, const NspInstance *nsp,
                     const Constraints *c, LapSolverFn solver);

void two_opt_backward(Schedule *s, const NspInstance *nsp,
                      const Constraints *c, LapSolverFn solver);

#endif
