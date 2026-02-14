#include "operator_registry.h"
#include "pcr.h"
#include "kswap.h"
#include "shift_relocate.h"
#include "swap_exchange.h"
#include "two_opt.h"
#include <stdio.h>
#include <string.h>

void registry_init(OperatorRegistry *reg) {
    reg->count = 0;
    memset(reg->entries, 0, sizeof(reg->entries));
}

int registry_add(OperatorRegistry *reg, const char *name, NeighbourhoodFn fn) {
    if (reg->count >= MAX_OPERATORS) return -1;
    reg->entries[reg->count].name = name;
    reg->entries[reg->count].fn = fn;
    reg->count++;
    return 0;
}

NeighbourhoodFn registry_get(const OperatorRegistry *reg, const char *name) {
    for (int i = 0; i < reg->count; i++)
        if (strcmp(reg->entries[i].name, name) == 0)
            return reg->entries[i].fn;
    return NULL;
}

void registry_list(const OperatorRegistry *reg) {
    printf("Registered operators (%d):\n", reg->count);
    for (int i = 0; i < reg->count; i++)
        printf("  [%d] %s\n", i, reg->entries[i].name);
}

static void w_shift_fwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { shift_relocate_forward(s, nsp, c, solver); }
static void w_shift_bwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { shift_relocate_backward(s, nsp, c, solver); }
static void w_swap_fwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { swap_exchange_forward(s, nsp, c, solver); }
static void w_swap_bwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { swap_exchange_backward(s, nsp, c, solver); }
static void w_2opt_fwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { two_opt_forward(s, nsp, c, solver); }
static void w_2opt_bwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { two_opt_backward(s, nsp, c, solver); }
static void w_pcr_fwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { pcr_forward(s, nsp, c, solver); }
static void w_pcr_bwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { pcr_backward(s, nsp, c, solver); }
static void w_k1_fwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { kswap_forward(s, nsp, c, solver, 1); }
static void w_k1_bwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { kswap_backward(s, nsp, c, solver, 1); }
static void w_k2_fwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { kswap_forward(s, nsp, c, solver, 2); }
static void w_k3_fwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { kswap_forward(s, nsp, c, solver, 3); }
static void w_k4_fwd(Schedule *s, const NspInstance *nsp, const Constraints *c, const LapSolverFn solver) { kswap_forward(s, nsp, c, solver, 4); }

void registry_load_defaults(OperatorRegistry *reg) {
    registry_init(reg);
    registry_add(reg, "shift_fwd",   w_shift_fwd);
    registry_add(reg, "shift_bwd",   w_shift_bwd);
    registry_add(reg, "swap_fwd",    w_swap_fwd);
    registry_add(reg, "swap_bwd",    w_swap_bwd);
    registry_add(reg, "2opt_fwd",    w_2opt_fwd);
    registry_add(reg, "2opt_bwd",    w_2opt_bwd);
    registry_add(reg, "pcr_fwd",     w_pcr_fwd);
    registry_add(reg, "pcr_bwd",     w_pcr_bwd);
    registry_add(reg, "kswap1_fwd",  w_k1_fwd);
    registry_add(reg, "kswap1_bwd",  w_k1_bwd);
    registry_add(reg, "kswap2_fwd",  w_k2_fwd);
    registry_add(reg, "kswap3_fwd",  w_k3_fwd);
    registry_add(reg, "kswap4_fwd",  w_k4_fwd);
}
