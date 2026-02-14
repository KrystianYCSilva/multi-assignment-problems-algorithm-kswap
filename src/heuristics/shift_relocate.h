#ifndef MAPA_SHIFT_RELOCATE_H
#define MAPA_SHIFT_RELOCATE_H

#include "../core/types.h"

void shift_relocate_forward(Schedule *s, const NspInstance *nsp,
                            const Constraints *c, LapSolverFn solver);

void shift_relocate_backward(Schedule *s, const NspInstance *nsp,
                             const Constraints *c, LapSolverFn solver);

#endif
