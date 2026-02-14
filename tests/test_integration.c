#include "test_framework.h"
#include "../src/core/types.h"
#include "../src/core/schedule.h"
#include "../src/core/cost.h"
#include "../src/solvers/lap_solver.h"
#include "../src/heuristics/constructive.h"
#include "../src/heuristics/pcr.h"
#include "../src/heuristics/kswap.h"

static NspInstance *create_small_nsp(void) {
    NspInstance *nsp = (NspInstance *)calloc(1, sizeof(NspInstance));
    nsp->size.n_nurses = 8;
    nsp->size.n_days = 7;
    nsp->size.n_shifts = 4;
    nsp->coverage_matrix = alloc_matrix_int(7, 4);
    for (int d = 0; d < 7; d++) {
        nsp->coverage_matrix[d][SHIFT_MORNING] = 2;
        nsp->coverage_matrix[d][SHIFT_EVENING] = 2;
        nsp->coverage_matrix[d][SHIFT_NIGHT] = 1;
        nsp->coverage_matrix[d][SHIFT_FREE] = 0;
    }
    nsp->preference_matrix = alloc_matrix_int(8, 7 * 4);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 28; j++)
            nsp->preference_matrix[i][j] = (i + j) % 5;
    return nsp;
}

static Constraints *create_small_constraints(void) {
    Constraints *c = (Constraints *)calloc(1, sizeof(Constraints));
    c->size.n_shifts = 4;
    c->number_of_assignments = (int *)calloc(2, sizeof(int));
    c->number_of_assignments[0] = 3;
    c->number_of_assignments[1] = 6;
    c->consecutive_working_shifts = (int *)calloc(2, sizeof(int));
    c->consecutive_working_shifts[0] = 1;
    c->consecutive_working_shifts[1] = 5;
    c->consecutive_assignments_matrix = alloc_matrix_int(4, 4);
    for (int s = 0; s < 4; s++) {
        c->consecutive_assignments_matrix[s][0] = 1;
        c->consecutive_assignments_matrix[s][1] = 4;
        c->consecutive_assignments_matrix[s][2] = 1;
        c->consecutive_assignments_matrix[s][3] = 5;
    }
    return c;
}

static void free_small_nsp(NspInstance *nsp) {
    free_matrix_int(nsp->coverage_matrix, 7);
    free_matrix_int(nsp->preference_matrix, 8);
    free(nsp);
}

static void free_small_constraints(Constraints *c) {
    free(c->number_of_assignments);
    free(c->consecutive_working_shifts);
    free_matrix_int(c->consecutive_assignments_matrix, 4);
    free(c);
}

static void test_constructive_builds_valid_schedule(void) {
    NspInstance *nsp = create_small_nsp();
    Constraints *c = create_small_constraints();
    Schedule *s = constructive_build(nsp, c, lap_solve_hungarian);
    ASSERT_NOT_NULL(s);
    ASSERT_EQ(s->size.n_nurses, 8);
    ASSERT_EQ(s->size.n_days, 7);
    for (int i = 0; i < 8; i++)
        for (int d = 0; d < 7; d++) {
            int sh = schedule_get(s, i, d);
            ASSERT_TRUE(sh >= 0 && sh < NUM_SHIFT_TYPES);
        }
    schedule_free(s);
    free_small_nsp(nsp);
    free_small_constraints(c);
}

static void test_cost_evaluation(void) {
    NspInstance *nsp = create_small_nsp();
    Constraints *c = create_small_constraints();
    Schedule *s = constructive_build(nsp, c, lap_solve_hungarian);
    int cost = cost_evaluate_silent(s, c, nsp);
    ASSERT_TRUE(cost >= 0);
    ASSERT_EQ(s->cost, cost);
    schedule_free(s);
    free_small_nsp(nsp);
    free_small_constraints(c);
}

static void test_pcr_improves_or_maintains(void) {
    NspInstance *nsp = create_small_nsp();
    Constraints *c = create_small_constraints();
    Schedule *s = constructive_build(nsp, c, lap_solve_hungarian);
    cost_evaluate_silent(s, c, nsp);
    int before = s->cost;
    pcr_forward(s, nsp, c, lap_solve_hungarian);
    ASSERT_TRUE(s->cost <= before);
    schedule_free(s);
    free_small_nsp(nsp);
    free_small_constraints(c);
}

static void test_kswap_improves_or_maintains(void) {
    NspInstance *nsp = create_small_nsp();
    Constraints *c = create_small_constraints();
    Schedule *s = constructive_build(nsp, c, lap_solve_hungarian);
    cost_evaluate_silent(s, c, nsp);
    int before = s->cost;
    kswap_forward(s, nsp, c, lap_solve_hungarian, 2);
    ASSERT_TRUE(s->cost <= before);
    schedule_free(s);
    free_small_nsp(nsp);
    free_small_constraints(c);
}

static void test_full_mapa_pipeline(void) {
    NspInstance *nsp = create_small_nsp();
    Constraints *c = create_small_constraints();
    Schedule *s = constructive_build(nsp, c, lap_solve_apc);
    cost_evaluate_silent(s, c, nsp);
    int initial_cost = s->cost;

    Schedule *best = schedule_copy(s);

    pcr_forward(s, nsp, c, lap_solve_apc);
    cost_evaluate_silent(s, c, nsp);
    if (s->cost < best->cost) { schedule_free(best); best = schedule_copy(s); }

    kswap_forward(s, nsp, c, lap_solve_apc, 1);
    cost_evaluate_silent(s, c, nsp);
    if (s->cost < best->cost) { schedule_free(best); best = schedule_copy(s); }

    pcr_backward(s, nsp, c, lap_solve_apc);
    cost_evaluate_silent(s, c, nsp);
    if (s->cost < best->cost) { schedule_free(best); best = schedule_copy(s); }

    kswap_backward(s, nsp, c, lap_solve_apc, 1);
    cost_evaluate_silent(s, c, nsp);
    if (s->cost < best->cost) { schedule_free(best); best = schedule_copy(s); }

    kswap_forward(s, nsp, c, lap_solve_apc, 3);
    cost_evaluate_silent(s, c, nsp);
    if (s->cost < best->cost) { schedule_free(best); best = schedule_copy(s); }

    kswap_forward(s, nsp, c, lap_solve_apc, 2);
    cost_evaluate_silent(s, c, nsp);
    if (s->cost < best->cost) { schedule_free(best); best = schedule_copy(s); }

    ASSERT_TRUE(best->cost <= initial_cost);
    printf("    initial=%d best=%d final=%d ", initial_cost, best->cost, s->cost);

    schedule_free(best);
    schedule_free(s);
    free_small_nsp(nsp);
    free_small_constraints(c);
}

int main(void) {
    printf("=== Integration Tests ===\n");
    RUN_TEST(test_constructive_builds_valid_schedule);
    RUN_TEST(test_cost_evaluation);
    RUN_TEST(test_pcr_improves_or_maintains);
    RUN_TEST(test_kswap_improves_or_maintains);
    RUN_TEST(test_full_mapa_pipeline);
    TEST_SUMMARY();
    return TEST_EXIT();
}
