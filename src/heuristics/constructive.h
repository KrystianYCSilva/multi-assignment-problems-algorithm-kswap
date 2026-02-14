#ifndef MAPA_CONSTRUCTIVE_H
#define MAPA_CONSTRUCTIVE_H

#include "../core/types.h"

Schedule *constructive_build(const NspInstance *nsp, const Constraints *c,
                             LapSolverFn solver);

#endif
