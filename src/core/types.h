#ifndef MAPA_TYPES_H
#define MAPA_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define SHIFT_MORNING 0
#define SHIFT_EVENING 1
#define SHIFT_NIGHT   2
#define SHIFT_FREE    3
#define NUM_SHIFT_TYPES 4
#define NUM_WORK_SHIFTS 3

#define PENALTY_HARD 100
#define PENALTY_SOFT 100

#define COST_INF (0x7FFFFFFF)

typedef struct {
    int n_nurses;
    int n_days;
    int n_shifts;
} ProblemSize;

typedef struct {
    ProblemSize size;
    int *number_of_assignments;
    int *consecutive_working_shifts;
    int **consecutive_assignments_matrix;
} Constraints;

typedef struct {
    ProblemSize size;
    int **coverage_matrix;
    int **preference_matrix;
} NspInstance;

typedef struct {
    ProblemSize size;
    int **nurse_schedule;
    int cost;
    int hard_violations;
    int soft_violations;
} Schedule;

typedef int (*LapSolverFn)(int n, int **cost, int *assignment);

typedef void (*NeighbourhoodFn)(Schedule *s, const NspInstance *nsp,
                                const Constraints *c, const LapSolverFn solver);

static inline int **alloc_matrix_int(int rows, int cols) {
    int **m = (int **)calloc((size_t)rows, sizeof(int *));
    if (!m) return NULL;
    for (int i = 0; i < rows; i++) {
        m[i] = (int *)calloc((size_t)cols, sizeof(int));
        if (!m[i]) {
            for (int j = 0; j < i; j++) free(m[j]);
            free(m);
            return NULL;
        }
    }
    return m;
}

static inline void free_matrix_int(int **m, int rows) {
    if (!m) return;
    for (int i = 0; i < rows; i++) free(m[i]);
    free(m);
}

#endif
