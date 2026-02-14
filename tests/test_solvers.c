#include "test_framework.h"
#include "../src/core/types.h"
#include "../src/solvers/lap_solver.h"

static void test_hungarian_identity(void) {
    int n = 3;
    int **cost = alloc_matrix_int(n, n);
    cost[0][0] = 1; cost[0][1] = 9; cost[0][2] = 9;
    cost[1][0] = 9; cost[1][1] = 1; cost[1][2] = 9;
    cost[2][0] = 9; cost[2][1] = 9; cost[2][2] = 1;
    int assign[3];
    int total = lap_solve_hungarian(n, cost, assign);
    ASSERT_EQ(total, 3);
    ASSERT_EQ(assign[0], 0);
    ASSERT_EQ(assign[1], 1);
    ASSERT_EQ(assign[2], 2);
    free_matrix_int(cost, n);
}

static void test_hungarian_permutation(void) {
    int n = 3;
    int **cost = alloc_matrix_int(n, n);
    cost[0][0] = 10; cost[0][1] = 5;  cost[0][2] = 13;
    cost[1][0] = 3;  cost[1][1] = 10; cost[1][2] = 18;
    cost[2][0] = 14; cost[2][1] = 9;  cost[2][2] = 7;
    int assign[3];
    int total = lap_solve_hungarian(n, cost, assign);
    ASSERT_EQ(total, 15);
    free_matrix_int(cost, n);
}

static void test_apc_identity(void) {
    int n = 3;
    int **cost = alloc_matrix_int(n, n);
    cost[0][0] = 1; cost[0][1] = 9; cost[0][2] = 9;
    cost[1][0] = 9; cost[1][1] = 1; cost[1][2] = 9;
    cost[2][0] = 9; cost[2][1] = 9; cost[2][2] = 1;
    int assign[3];
    int total = lap_solve_apc(n, cost, assign);
    ASSERT_EQ(total, 3);
    ASSERT_EQ(assign[0], 0);
    ASSERT_EQ(assign[1], 1);
    ASSERT_EQ(assign[2], 2);
    free_matrix_int(cost, n);
}

static void test_apc_permutation(void) {
    int n = 3;
    int **cost = alloc_matrix_int(n, n);
    cost[0][0] = 10; cost[0][1] = 5;  cost[0][2] = 13;
    cost[1][0] = 3;  cost[1][1] = 10; cost[1][2] = 18;
    cost[2][0] = 14; cost[2][1] = 9;  cost[2][2] = 7;
    int assign[3];
    int total = lap_solve_apc(n, cost, assign);
    ASSERT_EQ(total, 15);
    free_matrix_int(cost, n);
}

static void test_bap_basic(void) {
    int n = 3;
    int **cost = alloc_matrix_int(n, n);
    cost[0][0] = 1;  cost[0][1] = 5;  cost[0][2] = 10;
    cost[1][0] = 10; cost[1][1] = 2;  cost[1][2] = 5;
    cost[2][0] = 5;  cost[2][1] = 10; cost[2][2] = 3;
    int assign[3];
    int bottleneck = bap_solve(n, cost, assign);
    ASSERT_EQ(bottleneck, 3);
    free_matrix_int(cost, n);
}

static void test_hungarian_larger(void) {
    int n = 4;
    int **cost = alloc_matrix_int(n, n);
    cost[0][0] = 82; cost[0][1] = 83; cost[0][2] = 69; cost[0][3] = 92;
    cost[1][0] = 77; cost[1][1] = 37; cost[1][2] = 49; cost[1][3] = 92;
    cost[2][0] = 11; cost[2][1] = 69; cost[2][2] = 5;  cost[2][3] = 86;
    cost[3][0] = 8;  cost[3][1] = 9;  cost[3][2] = 98; cost[3][3] = 23;
    int assign[4];
    int total = lap_solve_hungarian(n, cost, assign);
    ASSERT_EQ(total, 140);
    free_matrix_int(cost, n);
}

static void test_solvers_agree(void) {
    int n = 4;
    int **cost = alloc_matrix_int(n, n);
    cost[0][0] = 82; cost[0][1] = 83; cost[0][2] = 69; cost[0][3] = 92;
    cost[1][0] = 77; cost[1][1] = 37; cost[1][2] = 49; cost[1][3] = 92;
    cost[2][0] = 11; cost[2][1] = 69; cost[2][2] = 5;  cost[2][3] = 86;
    cost[3][0] = 8;  cost[3][1] = 9;  cost[3][2] = 98; cost[3][3] = 23;
    int assign_h[4], assign_a[4];
    int total_h = lap_solve_hungarian(n, cost, assign_h);
    int total_a = lap_solve_apc(n, cost, assign_a);
    ASSERT_EQ(total_h, total_a);
    free_matrix_int(cost, n);
}

int main(void) {
    printf("=== Solver Tests ===\n");
    RUN_TEST(test_hungarian_identity);
    RUN_TEST(test_hungarian_permutation);
    RUN_TEST(test_apc_identity);
    RUN_TEST(test_apc_permutation);
    RUN_TEST(test_bap_basic);
    RUN_TEST(test_hungarian_larger);
    RUN_TEST(test_solvers_agree);
    TEST_SUMMARY();
    return TEST_EXIT();
}
