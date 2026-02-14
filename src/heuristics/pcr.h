#ifndef MAPA_PCR_H
#define MAPA_PCR_H

#include "../core/types.h"

void pcr_forward(Schedule *s, const NspInstance *nsp,
                 const Constraints *c, LapSolverFn solver);

void pcr_backward(Schedule *s, const NspInstance *nsp,
                  const Constraints *c, LapSolverFn solver);

#endif
