#include "benchmark.h"
#include "../core/cost.h"
#include "../core/schedule.h"
#include <stdio.h>
#include <time.h>

BenchmarkReport *benchmark_create(int capacity) {
    BenchmarkReport *r = (BenchmarkReport *)calloc(1, sizeof(BenchmarkReport));
    if (!r) return NULL;
    r->results = (BenchmarkResult *)calloc((size_t)capacity, sizeof(BenchmarkResult));
    r->count = 0;
    r->capacity = capacity;
    return r;
}

void benchmark_free(BenchmarkReport *report) {
    if (!report) return;
    free(report->results);
    free(report);
}

void benchmark_run_operator(BenchmarkReport *report,
                            const char *name,
                            NeighbourhoodFn fn,
                            Schedule *s,
                            const NspInstance *nsp,
                            const Constraints *c,
                            LapSolverFn solver) {
    if (report->count >= report->capacity) return;

    cost_evaluate_silent(s, c, nsp);
    int before = s->cost;

    clock_t tic = clock();
    fn(s, nsp, c, solver);
    cost_evaluate_silent(s, c, nsp);
    clock_t toc = clock();

    BenchmarkResult *res = &report->results[report->count++];
    res->operator_name = name;
    res->initial_cost = before;
    res->final_cost = s->cost;
    res->hard_violations = s->hard_violations;
    res->soft_violations = s->soft_violations;
    res->elapsed_sec = (double)(toc - tic) / CLOCKS_PER_SEC;
    res->improvement = before - s->cost;
}

void benchmark_print_report(const BenchmarkReport *report) {
    printf("%-20s %8s %8s %8s %6s %10s\n",
           "Operator", "Before", "After", "Improv", "HCV", "Time(s)");
    printf("--------------------------------------------------------------\n");
    for (int i = 0; i < report->count; i++) {
        const BenchmarkResult *r = &report->results[i];
        printf("%-20s %8d %8d %8d %6d %10.4f\n",
               r->operator_name, r->initial_cost, r->final_cost,
               r->improvement, r->hard_violations, r->elapsed_sec);
    }
}

int benchmark_export_csv(const BenchmarkReport *report, const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) return -1;
    fprintf(fp, "operator;before;after;improvement;hcv;scv;time_sec\n");
    for (int i = 0; i < report->count; i++) {
        const BenchmarkResult *r = &report->results[i];
        fprintf(fp, "%s;%d;%d;%d;%d;%d;%.6f\n",
                r->operator_name, r->initial_cost, r->final_cost,
                r->improvement, r->hard_violations, r->soft_violations,
                r->elapsed_sec);
    }
    fclose(fp);
    return 0;
}
