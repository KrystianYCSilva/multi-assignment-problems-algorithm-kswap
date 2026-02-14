#ifndef MAPA_SWAP_EXCHANGE_H
#define MAPA_SWAP_EXCHANGE_H

#include "../core/types.h"

void swap_exchange_forward(Schedule *s, const NspInstance *nsp,
                           const Constraints *c, LapSolverFn solver);

void swap_exchange_backward(Schedule *s, const NspInstance *nsp,
                            const Constraints *c, LapSolverFn solver);

#endif
