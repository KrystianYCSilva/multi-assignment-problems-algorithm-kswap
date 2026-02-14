#ifndef MAPA_COST_H
#define MAPA_COST_H

#include "types.h"

int cost_evaluate(Schedule *s, const Constraints *c, const NspInstance *nsp);

int cost_evaluate_silent(Schedule *s, const Constraints *c, const NspInstance *nsp);

int cost_partial_nurse(const Schedule *s, const Constraints *c,
                       const NspInstance *nsp, int nurse);

bool is_forbidden_sequence(int prev_shift, int next_shift);

int count_consecutive_work(const Schedule *s, int nurse);

int count_consecutive_same_shift(const Schedule *s, int nurse, int shift);

int count_total_assignments(const Schedule *s, int nurse);

#endif
