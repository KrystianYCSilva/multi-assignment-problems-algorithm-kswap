#ifndef MAPA_KSWAP_H
#define MAPA_KSWAP_H

#include "../core/types.h"

void kswap_forward(Schedule *s, const NspInstance *nsp,
                   const Constraints *c, LapSolverFn solver, int k);

void kswap_backward(Schedule *s, const NspInstance *nsp,
                    const Constraints *c, LapSolverFn solver, int k);

#endif
