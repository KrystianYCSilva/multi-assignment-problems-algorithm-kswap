#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "core/types.h"
#include "core/schedule.h"
#include "core/cost.h"
#include "io/nsp_reader.h"
#include "io/result_writer.h"
#include "solvers/lap_solver.h"
#include "heuristics/constructive.h"
#include "heuristics/pcr.h"
#include "heuristics/kswap.h"
#include "heuristics/vns.h"
#include "heuristics/vnd.h"
#include "heuristics/operator_registry.h"

typedef enum {
    SOLVER_APC,
    SOLVER_HUNGARIAN
} SolverType;

typedef enum {
    MODE_MAPA,
    MODE_MAPA_VND,
    MODE_VNS,
    MODE_VNS_BAP,
    MODE_VNS_VND
} RunMode;

typedef struct {
    int n_nurses;
    int n_days;
    int n_shifts;
    int max_files;
    int start_file;
    int constraint_id;
    int max_iterations;
    SolverType solver;
    RunMode mode;
    bool verbose;
    char nsp_dir[256];
    char constraint_dir[256];
    char result_dir[256];
} Config;

static Config default_config(void) {
    Config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.n_nurses = 50;
    cfg.n_days = 7;
    cfg.n_shifts = 4;
    cfg.max_files = 7290;
    cfg.start_file = 1;
    cfg.constraint_id = 6;
    cfg.max_iterations = 5;
    cfg.solver = SOLVER_APC;
    cfg.mode = MODE_MAPA;
    cfg.verbose = false;
    strncpy(cfg.nsp_dir, "./files/N50", sizeof(cfg.nsp_dir) - 1);
    strncpy(cfg.constraint_dir, "./files/casos-1-8", sizeof(cfg.constraint_dir) - 1);
    strncpy(cfg.result_dir, "./resultados/N50", sizeof(cfg.result_dir) - 1);
    return cfg;
}

static void print_help(void) {
    printf("MAPA Nurse Scheduling - Pilot Execution\n\n");
    printf("Usage: mapa_kswap [options]\n\n");
    printf("Instance:\n");
    printf("  --nurses N          Number of nurses (default: 50)\n");
    printf("  --days N            Number of days (default: 7)\n");
    printf("  --max-files N       Max instance files (default: 7290)\n");
    printf("  --start N           Starting file number (default: 1)\n");
    printf("  --constraint N      Constraint file id (default: 6)\n");
    printf("  --nsp-dir PATH      NSP instances directory\n");
    printf("  --constraint-dir P  Constraints directory\n");
    printf("  --result-dir PATH   Results output directory\n\n");
    printf("Algorithm:\n");
    printf("  --solver TYPE       apc|hungarian (default: apc)\n");
    printf("  --iterations N      Max no-improve iterations (default: 5)\n");
    printf("  --mode MODE         Execution mode:\n");
    printf("      mapa            Classic MAPA (PCR + kSwap, no VND)\n");
    printf("      mapa-vnd        MAPA with VND local search\n");
    printf("      vns             VNS metaheuristic\n");
    printf("      vns-bap         VNS with BAP solver\n");
    printf("      vns-vnd         VNS with VND as local search\n\n");
    printf("  --verbose           Show VND/VNS step details\n");
    printf("  --list-operators    List all registered operators\n");
    printf("  --help              Show this help\n");
}

static void parse_args(int argc, char **argv, Config *cfg) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--nurses") == 0 && i + 1 < argc)
            cfg->n_nurses = atoi(argv[++i]);
        else if (strcmp(argv[i], "--days") == 0 && i + 1 < argc)
            cfg->n_days = atoi(argv[++i]);
        else if (strcmp(argv[i], "--max-files") == 0 && i + 1 < argc)
            cfg->max_files = atoi(argv[++i]);
        else if (strcmp(argv[i], "--start") == 0 && i + 1 < argc)
            cfg->start_file = atoi(argv[++i]);
        else if (strcmp(argv[i], "--constraint") == 0 && i + 1 < argc)
            cfg->constraint_id = atoi(argv[++i]);
        else if (strcmp(argv[i], "--iterations") == 0 && i + 1 < argc)
            cfg->max_iterations = atoi(argv[++i]);
        else if (strcmp(argv[i], "--nsp-dir") == 0 && i + 1 < argc)
            strncpy(cfg->nsp_dir, argv[++i], sizeof(cfg->nsp_dir) - 1);
        else if (strcmp(argv[i], "--constraint-dir") == 0 && i + 1 < argc)
            strncpy(cfg->constraint_dir, argv[++i], sizeof(cfg->constraint_dir) - 1);
        else if (strcmp(argv[i], "--result-dir") == 0 && i + 1 < argc)
            strncpy(cfg->result_dir, argv[++i], sizeof(cfg->result_dir) - 1);
        else if (strcmp(argv[i], "--verbose") == 0)
            cfg->verbose = true;
        else if (strcmp(argv[i], "--solver") == 0 && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "hungarian") == 0) cfg->solver = SOLVER_HUNGARIAN;
            else cfg->solver = SOLVER_APC;
        }
        else if (strcmp(argv[i], "--mode") == 0 && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "mapa-vnd") == 0) cfg->mode = MODE_MAPA_VND;
            else if (strcmp(argv[i], "vns") == 0) cfg->mode = MODE_VNS;
            else if (strcmp(argv[i], "vns-bap") == 0) cfg->mode = MODE_VNS_BAP;
            else if (strcmp(argv[i], "vns-vnd") == 0) cfg->mode = MODE_VNS_VND;
            else cfg->mode = MODE_MAPA;
        }
        else if (strcmp(argv[i], "--list-operators") == 0) {
            OperatorRegistry reg;
            registry_load_defaults(&reg);
            registry_list(&reg);
            exit(0);
        }
        else if (strcmp(argv[i], "--help") == 0) {
            print_help();
            exit(0);
        }
    }
}

static const char *mode_name(RunMode m) {
    switch (m) {
        case MODE_MAPA:     return "mapa";
        case MODE_MAPA_VND: return "mapa-vnd";
        case MODE_VNS:      return "vns";
        case MODE_VNS_BAP:  return "vns-bap";
        case MODE_VNS_VND:  return "vns-vnd";
    }
    return "unknown";
}

static Schedule *mapa_improve(Schedule *initial, const NspInstance *nsp,
                               const Constraints *c, LapSolverFn solver,
                               int max_no_improve) {
    Schedule *best = schedule_copy(initial);
    Schedule *current = schedule_copy(initial);
    cost_evaluate_silent(current, c, nsp);
    cost_evaluate_silent(best, c, nsp);

    int no_improve = 0;
    while (no_improve < max_no_improve) {
        Schedule *candidate = schedule_copy(current);

        pcr_forward(candidate, nsp, c, solver);
        kswap_forward(candidate, nsp, c, solver, 1);
        pcr_backward(candidate, nsp, c, solver);
        kswap_backward(candidate, nsp, c, solver, 1);

        kswap_forward(candidate, nsp, c, solver, 4);
        kswap_forward(candidate, nsp, c, solver, 3);
        kswap_forward(candidate, nsp, c, solver, 2);

        cost_evaluate_silent(candidate, c, nsp);

        if (candidate->cost < current->cost) {
            schedule_free(current);
            current = candidate;
            no_improve = 0;
            if (current->cost < best->cost) {
                schedule_free(best);
                best = schedule_copy(current);
            }
        } else {
            schedule_free(candidate);
            no_improve++;
        }
    }

    schedule_free(current);
    return best;
}

static Schedule *mapa_vnd_improve(Schedule *initial, const NspInstance *nsp,
                                   const Constraints *c, LapSolverFn solver,
                                   int max_no_improve, bool verbose) {
    Schedule *best = schedule_copy(initial);
    Schedule *current = schedule_copy(initial);
    cost_evaluate_silent(current, c, nsp);
    cost_evaluate_silent(best, c, nsp);

    VndConfig vnd_cfg;
    vnd_load_default(&vnd_cfg);
    vnd_cfg.verbose = verbose;

    int no_improve = 0;
    while (no_improve < max_no_improve) {
        Schedule *candidate = schedule_copy(current);

        vnd_solve(candidate, nsp, c, solver, &vnd_cfg);
        cost_evaluate_silent(candidate, c, nsp);

        if (candidate->cost < current->cost) {
            schedule_free(current);
            current = candidate;
            no_improve = 0;
            if (current->cost < best->cost) {
                schedule_free(best);
                best = schedule_copy(current);
            }
        } else {
            schedule_free(candidate);
            no_improve++;
        }
    }

    schedule_free(current);
    return best;
}

static Schedule *vns_vnd_improve(Schedule *initial, const NspInstance *nsp,
                                  const Constraints *c, LapSolverFn solver,
                                  int max_no_improve, bool verbose) {
    Schedule *best = schedule_copy(initial);
    Schedule *current = schedule_copy(initial);
    cost_evaluate_silent(current, c, nsp);
    cost_evaluate_silent(best, c, nsp);

    VndConfig vnd_cfg;
    vnd_load_default(&vnd_cfg);
    vnd_cfg.verbose = verbose;

    int no_improve = 0;
    int iter = 0;
    while (no_improve < max_no_improve) {
        Schedule *candidate = schedule_copy(current);

        int k_shaking = (no_improve % 4) + 1;
        kswap_forward(candidate, nsp, c, solver, k_shaking);

        vnd_solve(candidate, nsp, c, solver, &vnd_cfg);
        cost_evaluate_silent(candidate, c, nsp);

        if (candidate->cost < current->cost) {
            schedule_free(current);
            current = candidate;
            no_improve = 0;
            if (current->cost < best->cost) {
                schedule_free(best);
                best = schedule_copy(current);
            }
        } else {
            schedule_free(candidate);
            no_improve++;
        }

        if (verbose)
            printf("VNS-VND iter=%d cost=%d best=%d no_improve=%d shaking=k%d\n",
                   iter, current->cost, best->cost, no_improve, k_shaking);
        iter++;
    }

    schedule_free(current);
    return best;
}

int main(int argc, char **argv) {
    Config cfg = default_config();
    parse_args(argc, argv, &cfg);

    if (cfg.result_dir[0] == '\0' || strcmp(cfg.result_dir, "./resultados/N50") == 0) {
        snprintf(cfg.result_dir, sizeof(cfg.result_dir), "./resultados/N%d", cfg.n_nurses);
    }

    LapSolverFn solver = (cfg.solver == SOLVER_HUNGARIAN) ?
                          lap_solve_hungarian : lap_solve_apc;

    printf("=== MAPA NSP Solver ===\n");
    printf("Mode: %s | Solver: %s | Iterations: %d\n",
           mode_name(cfg.mode),
           cfg.solver == SOLVER_HUNGARIAN ? "hungarian" : "apc",
           cfg.max_iterations);

    char constraint_path[512];
    snprintf(constraint_path, sizeof(constraint_path), "%s/%d.gen",
             cfg.constraint_dir, cfg.constraint_id);

    Constraints *constraints = constraints_read(constraint_path);
    if (!constraints) {
        fprintf(stderr, "Failed to read constraints from %s\n", constraint_path);
        return 1;
    }
    constraints_print(constraints);

    char result_path[512];
    snprintf(result_path, sizeof(result_path), "%s/Constraint_%d_%s.csv",
             cfg.result_dir, cfg.constraint_id, mode_name(cfg.mode));

    for (int file = cfg.start_file; file <= cfg.max_files; file++) {
        char nsp_path[512];
        snprintf(nsp_path, sizeof(nsp_path), "%s/%d.nsp", cfg.nsp_dir, file);

        NspInstance *nsp = nsp_read(nsp_path);
        if (!nsp) continue;

        clock_t tic = clock();

        Schedule *initial = constructive_build(nsp, constraints, solver);
        cost_evaluate_silent(initial, constraints, nsp);
        int initial_cost = initial->cost;

        Schedule *result = NULL;

        switch (cfg.mode) {
            case MODE_MAPA:
                result = mapa_improve(initial, nsp, constraints, solver,
                                      cfg.max_iterations);
                break;
            case MODE_MAPA_VND:
                result = mapa_vnd_improve(initial, nsp, constraints, solver,
                                          cfg.max_iterations, cfg.verbose);
                break;
            case MODE_VNS: {
                VnsConfig vns_cfg = vns_default_config();
                vns_cfg.max_no_improve = cfg.max_iterations;
                vns_cfg.use_bap = false;
                result = vns_solve(initial, nsp, constraints, solver, solver, &vns_cfg);
                break;
            }
            case MODE_VNS_BAP: {
                VnsConfig vns_cfg = vns_default_config();
                vns_cfg.max_no_improve = cfg.max_iterations;
                vns_cfg.use_bap = true;
                result = vns_solve(initial, nsp, constraints, solver,
                                   bap_solve, &vns_cfg);
                break;
            }
            case MODE_VNS_VND:
                result = vns_vnd_improve(initial, nsp, constraints, solver,
                                         cfg.max_iterations, cfg.verbose);
                break;
        }

        cost_evaluate_silent(result, constraints, nsp);

        clock_t toc = clock();
        double elapsed = (double)(toc - tic) / CLOCKS_PER_SEC;

        char instance_name[64];
        snprintf(instance_name, sizeof(instance_name), "%d.nsp", file);

        result_write_csv(result_path, instance_name, constraint_path,
                         cfg.max_iterations, initial_cost, result, elapsed);

        printf("[%s] File %d: initial=%d final=%d hcv=%d scv=%d time=%.3fs\n",
               mode_name(cfg.mode), file, initial_cost, result->cost,
               result->hard_violations, result->soft_violations, elapsed);

        schedule_free(initial);
        schedule_free(result);
        nsp_free(nsp);
    }

    constraints_free(constraints);
    return 0;
}
