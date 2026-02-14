#include "schedule.h"
#include <stdio.h>
#include <string.h>

Schedule *schedule_create(int n_nurses, int n_days) {
    Schedule *s = (Schedule *)calloc(1, sizeof(Schedule));
    if (!s) return NULL;
    s->size.n_nurses = n_nurses;
    s->size.n_days = n_days;
    s->size.n_shifts = NUM_SHIFT_TYPES;
    s->nurse_schedule = alloc_matrix_int(n_nurses, n_days);
    if (!s->nurse_schedule) {
        free(s);
        return NULL;
    }
    for (int i = 0; i < n_nurses; i++)
        for (int d = 0; d < n_days; d++)
            s->nurse_schedule[i][d] = SHIFT_FREE;
    s->cost = 0;
    s->hard_violations = 0;
    s->soft_violations = 0;
    return s;
}

Schedule *schedule_copy(const Schedule *s) {
    if (!s) return NULL;
    int n = s->size.n_nurses;
    int d = s->size.n_days;
    Schedule *copy = schedule_create(n, d);
    if (!copy) return NULL;
    for (int i = 0; i < n; i++)
        memcpy(copy->nurse_schedule[i], s->nurse_schedule[i], (size_t)d * sizeof(int));
    copy->cost = s->cost;
    copy->hard_violations = s->hard_violations;
    copy->soft_violations = s->soft_violations;
    return copy;
}

void schedule_free(Schedule *s) {
    if (!s) return;
    free_matrix_int(s->nurse_schedule, s->size.n_nurses);
    free(s);
}

int schedule_get(const Schedule *s, int nurse, int day) {
    return s->nurse_schedule[nurse][day];
}

void schedule_set(Schedule *s, int nurse, int day, int shift) {
    s->nurse_schedule[nurse][day] = shift;
}

void schedule_swap_block(Schedule *s, int nurse_a, int nurse_b,
                         int day_start, int day_end) {
    for (int d = day_start; d <= day_end && d < s->size.n_days; d++) {
        int tmp = s->nurse_schedule[nurse_a][d];
        s->nurse_schedule[nurse_a][d] = s->nurse_schedule[nurse_b][d];
        s->nurse_schedule[nurse_b][d] = tmp;
    }
}

void schedule_print(const Schedule *s) {
    printf("Schedule [%d nurses x %d days] cost=%d hv=%d sv=%d\n",
           s->size.n_nurses, s->size.n_days, s->cost,
           s->hard_violations, s->soft_violations);
    const char *labels[] = {"M", "E", "N", "F"};
    for (int i = 0; i < s->size.n_nurses; i++) {
        printf("  Nurse %3d: ", i);
        for (int d = 0; d < s->size.n_days; d++) {
            int sh = s->nurse_schedule[i][d];
            if (sh >= 0 && sh < NUM_SHIFT_TYPES)
                printf("%s ", labels[sh]);
            else
                printf("? ");
        }
        printf("\n");
    }
}

int schedule_count_shift_on_day(const Schedule *s, int day, int shift) {
    int count = 0;
    for (int i = 0; i < s->size.n_nurses; i++)
        if (s->nurse_schedule[i][day] == shift)
            count++;
    return count;
}

void schedule_reverse_days(Schedule *s) {
    int n = s->size.n_nurses;
    int d = s->size.n_days;
    for (int i = 0; i < n; i++) {
        for (int left = 0, right = d - 1; left < right; left++, right--) {
            int tmp = s->nurse_schedule[i][left];
            s->nurse_schedule[i][left] = s->nurse_schedule[i][right];
            s->nurse_schedule[i][right] = tmp;
        }
    }
}
