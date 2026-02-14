#ifndef MAPA_VND_H
#define MAPA_VND_H

#include "../core/types.h"

#define VND_MAX_NEIGHBOURHOODS 16

typedef struct {
    NeighbourhoodFn neighbourhoods[VND_MAX_NEIGHBOURHOODS];
    const char *names[VND_MAX_NEIGHBOURHOODS];
    int count;
    bool verbose;
} VndConfig;

void vnd_init(VndConfig *cfg);
int  vnd_add(VndConfig *cfg, const char *name, NeighbourhoodFn fn);
void vnd_load_default(VndConfig *cfg);

void vnd_solve(Schedule *s, const NspInstance *nsp, const Constraints *c,
               LapSolverFn solver, const VndConfig *cfg);

#endif
