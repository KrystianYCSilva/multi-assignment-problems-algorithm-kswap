#include "vnd.h"
#include "../core/cost.h"
#include "../core/schedule.h"
#include "shift_relocate.h"
#include "swap_exchange.h"
#include "two_opt.h"
#include "kswap.h"
#include "pcr.h"
#include <stdio.h>
#include <string.h>

void vnd_init(VndConfig *cfg) {
    memset(cfg, 0, sizeof(VndConfig));
    cfg->count = 0;
    cfg->verbose = false;
}

int vnd_add(VndConfig *cfg, const char *name, NeighbourhoodFn fn) {
    if (cfg->count >= VND_MAX_NEIGHBOURHOODS) return -1;
    cfg->names[cfg->count] = name;
    cfg->neighbourhoods[cfg->count] = fn;
    cfg->count++;
    return 0;
}

static void wrap_shift(Schedule *s, const NspInstance *nsp,
                       const Constraints *c, const LapSolverFn solver) {
    shift_relocate_forward(s, nsp, c, solver);
}

static void wrap_swap(Schedule *s, const NspInstance *nsp,
                      const Constraints *c, const LapSolverFn solver) {
    swap_exchange_forward(s, nsp, c, solver);
}

static void wrap_2opt(Schedule *s, const NspInstance *nsp,
                      const Constraints *c, const LapSolverFn solver) {
    two_opt_forward(s, nsp, c, solver);
}

static void wrap_kswap1(Schedule *s, const NspInstance *nsp,
                        const Constraints *c, const LapSolverFn solver) {
    kswap_forward(s, nsp, c, solver, 1);
}

static void wrap_kswap2(Schedule *s, const NspInstance *nsp,
                        const Constraints *c, const LapSolverFn solver) {
    kswap_forward(s, nsp, c, solver, 2);
}

static void wrap_kswap3(Schedule *s, const NspInstance *nsp,
                        const Constraints *c, const LapSolverFn solver) {
    kswap_forward(s, nsp, c, solver, 3);
}

static void wrap_kswap4(Schedule *s, const NspInstance *nsp,
                        const Constraints *c, const LapSolverFn solver) {
    kswap_forward(s, nsp, c, solver, 4);
}

static void wrap_pcr(Schedule *s, const NspInstance *nsp,
                     const Constraints *c, const LapSolverFn solver) {
    pcr_forward(s, nsp, c, solver);
}

void vnd_load_default(VndConfig *cfg) {
    vnd_init(cfg);
    vnd_add(cfg, "N1:shift_relocate", wrap_shift);
    vnd_add(cfg, "N2:swap_exchange",  wrap_swap);
    vnd_add(cfg, "N3:2opt",           wrap_2opt);
    vnd_add(cfg, "N4:kswap1(PRT)",    wrap_kswap1);
    vnd_add(cfg, "N5:kswap2",         wrap_kswap2);
    vnd_add(cfg, "N6:kswap3",         wrap_kswap3);
    vnd_add(cfg, "N7:kswap4",         wrap_kswap4);
    vnd_add(cfg, "N8:pcr(CRP)",       wrap_pcr);
}

void vnd_solve(Schedule *s, const NspInstance *nsp, const Constraints *c,
               LapSolverFn solver, const VndConfig *cfg) {
    cost_evaluate_silent(s, c, nsp);
    int k = 0;

    while (k < cfg->count) {
        int before = s->cost;

        cfg->neighbourhoods[k](s, nsp, c, solver);
        cost_evaluate_silent(s, c, nsp);

        if (s->cost < before) {
            if (cfg->verbose)
                printf("  VND: %s improved %d -> %d (restart)\n",
                       cfg->names[k], before, s->cost);
            k = 0;
        } else {
            if (cfg->verbose)
                printf("  VND: %s no improvement at cost=%d\n",
                       cfg->names[k], s->cost);
            k++;
        }
    }
}
