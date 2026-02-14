#ifndef MAPA_BENCHMARK_H
#define MAPA_BENCHMARK_H

#include "../core/types.h"

typedef struct {
    const char *operator_name;
    int initial_cost;
    int final_cost;
    int hard_violations;
    int soft_violations;
    double elapsed_sec;
    int improvement;
} BenchmarkResult;

typedef struct {
    BenchmarkResult *results;
    int count;
    int capacity;
} BenchmarkReport;

BenchmarkReport *benchmark_create(int capacity);
void benchmark_free(BenchmarkReport *report);

void benchmark_run_operator(BenchmarkReport *report,
                            const char *name,
                            NeighbourhoodFn fn,
                            Schedule *s,
                            const NspInstance *nsp,
                            const Constraints *c,
                            LapSolverFn solver);

void benchmark_print_report(const BenchmarkReport *report);
int  benchmark_export_csv(const BenchmarkReport *report, const char *filepath);

#endif
