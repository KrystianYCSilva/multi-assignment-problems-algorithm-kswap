#ifndef MAPA_OPERATOR_REGISTRY_H
#define MAPA_OPERATOR_REGISTRY_H

#include "../core/types.h"

#define MAX_OPERATORS 32

typedef struct {
    const char *name;
    NeighbourhoodFn fn;
} OperatorEntry;

typedef struct {
    OperatorEntry entries[MAX_OPERATORS];
    int count;
} OperatorRegistry;

void registry_init(OperatorRegistry *reg);
int  registry_add(OperatorRegistry *reg, const char *name, NeighbourhoodFn fn);
NeighbourhoodFn registry_get(const OperatorRegistry *reg, const char *name);
void registry_list(const OperatorRegistry *reg);
void registry_load_defaults(OperatorRegistry *reg);

#endif
