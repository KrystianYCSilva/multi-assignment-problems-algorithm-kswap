#ifndef MAPA_SCHEDULE_H
#define MAPA_SCHEDULE_H

#include "types.h"

Schedule *schedule_create(int n_nurses, int n_days);
Schedule *schedule_copy(const Schedule *s);
void schedule_free(Schedule *s);

int schedule_get(const Schedule *s, int nurse, int day);
void schedule_set(Schedule *s, int nurse, int day, int shift);

void schedule_swap_block(Schedule *s, int nurse_a, int nurse_b,
                         int day_start, int day_end);

void schedule_print(const Schedule *s);

int schedule_count_shift_on_day(const Schedule *s, int day, int shift);

void schedule_reverse_days(Schedule *s);

#endif
