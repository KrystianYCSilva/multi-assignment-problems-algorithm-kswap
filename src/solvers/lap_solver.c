#include "lap_solver.h"
#include <string.h>
#include <stdio.h>

static void hungarian_step_init(int n, int **cost, int *u, int *v,
                                int *col_mate, int *row_mate,
                                int *parent_row, int *unchosen_row,
                                int *slack, int *slack_row) {
    for (int j = 0; j < n; j++) {
        v[j] = COST_INF;
        col_mate[j] = -1;
    }
    for (int i = 0; i < n; i++) {
        u[i] = COST_INF;
        row_mate[i] = -1;
        for (int j = 0; j < n; j++) {
            if (cost[i][j] < u[i]) u[i] = cost[i][j];
        }
        for (int j = 0; j < n; j++) {
            if (cost[i][j] == u[i] && col_mate[j] == -1) {
                row_mate[i] = j;
                col_mate[j] = i;
                break;
            }
        }
    }
    for (int j = 0; j < n; j++) {
        v[j] = COST_INF;
        for (int i = 0; i < n; i++) {
            int rc = cost[i][j] - u[i];
            if (rc < v[j]) v[j] = rc;
        }
    }
    (void)parent_row;
    (void)unchosen_row;
    (void)slack;
    (void)slack_row;
}

int lap_solve_hungarian(int n, int **cost, int *assignment) {
    if (n <= 0) return 0;

    int *u = (int *)calloc((size_t)n, sizeof(int));
    int *v = (int *)calloc((size_t)n, sizeof(int));
    int *row_mate = (int *)calloc((size_t)n, sizeof(int));
    int *col_mate = (int *)calloc((size_t)n, sizeof(int));
    int *parent_row = (int *)calloc((size_t)n, sizeof(int));
    int *unchosen_row = (int *)calloc((size_t)n, sizeof(int));
    int *slack = (int *)calloc((size_t)n, sizeof(int));
    int *slack_row = (int *)calloc((size_t)n, sizeof(int));

    hungarian_step_init(n, cost, u, v, col_mate, row_mate,
                        parent_row, unchosen_row, slack, slack_row);

    int unmatched = 0;
    for (int i = 0; i < n; i++)
        if (row_mate[i] == -1)
            unchosen_row[unmatched++] = i;

    while (unmatched > 0) {
        int num_unchosen = unmatched;
        bool found = false;
        int s_col = -1;

        for (int j = 0; j < n; j++) {
            slack[j] = COST_INF;
            slack_row[j] = -1;
            parent_row[j] = -1;
        }

        for (int t = 0; t < num_unchosen; t++) {
            int i = unchosen_row[t];
            for (int j = 0; j < n; j++) {
                int val = cost[i][j] - u[i] - v[j];
                if (val < slack[j]) {
                    slack[j] = val;
                    slack_row[j] = i;
                }
            }
        }

        while (!found) {
            int min_slack = COST_INF;
            s_col = -1;
            for (int j = 0; j < n; j++) {
                if (parent_row[j] == -1 && slack[j] < min_slack) {
                    min_slack = slack[j];
                    s_col = j;
                }
            }

            if (s_col == -1) break;

            for (int t = 0; t < num_unchosen; t++) {
                int i = unchosen_row[t];
                u[i] += min_slack;
            }
            for (int j = 0; j < n; j++) {
                if (parent_row[j] != -1) {
                    v[j] -= min_slack;
                } else {
                    slack[j] -= min_slack;
                }
            }

            for (int j = 0; j < n; j++) {
                if (parent_row[j] == -1 && slack[j] == 0) {
                    parent_row[j] = slack_row[j];
                    if (col_mate[j] == -1) {
                        s_col = j;
                        found = true;
                        break;
                    } else {
                        unchosen_row[num_unchosen++] = col_mate[j];
                        int i = col_mate[j];
                        for (int jj = 0; jj < n; jj++) {
                            if (parent_row[jj] == -1) {
                                int val = cost[i][jj] - u[i] - v[jj];
                                if (val < slack[jj]) {
                                    slack[jj] = val;
                                    slack_row[jj] = i;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (s_col != -1) {
            while (s_col != -1) {
                int i = parent_row[s_col];
                int prev_col = row_mate[i];
                col_mate[s_col] = i;
                row_mate[i] = s_col;
                s_col = prev_col;
            }
        }

        unmatched = 0;
        for (int i = 0; i < n; i++)
            if (row_mate[i] == -1)
                unchosen_row[unmatched++] = i;
    }

    int total = 0;
    for (int i = 0; i < n; i++) {
        assignment[i] = row_mate[i];
        if (assignment[i] >= 0 && assignment[i] < n)
            total += cost[i][assignment[i]];
    }

    free(u); free(v); free(row_mate); free(col_mate);
    free(parent_row); free(unchosen_row); free(slack); free(slack_row);

    return total;
}

int lap_solve_apc(int n, int **cost, int *assignment) {
    if (n <= 0) return 0;

    int *u = (int *)malloc((size_t)n * sizeof(int));
    int *v = (int *)malloc((size_t)n * sizeof(int));
    int *fb = (int *)malloc((size_t)n * sizeof(int));
    int *f = (int *)malloc((size_t)n * sizeof(int));
    int *rc = (int *)malloc((size_t)n * sizeof(int));
    int *pi = (int *)malloc((size_t)n * sizeof(int));
    int *lr = (int *)malloc((size_t)n * sizeof(int));
    int *uc = (int *)malloc((size_t)n * sizeof(int));
    int *p = (int *)malloc((size_t)n * sizeof(int));

    if (!u || !v || !fb || !f || !rc || !pi || !lr || !uc || !p) {
        free(u); free(v); free(fb); free(f);
        free(rc); free(pi); free(lr); free(uc); free(p);
        return -1;
    }

    int m = 0, r = 0;
    for (int k = 0; k < n; k++) { f[k] = -1; fb[k] = -1; }

    for (int j = 0; j < n; j++) {
        int min_val = COST_INF;
        for (int i = 0; i < n; i++) {
            int a = cost[i][j];
            if (a < min_val || (a == min_val && f[i] < 0)) {
                min_val = a; r = i;
            }
        }
        v[j] = min_val;
        if (f[r] < 0) { m++; fb[j] = r; f[r] = j; u[r] = 0; p[r] = j + 1; }
    }

    for (int i = 0; i < n; i++) {
        if (f[i] < 0) {
            int min_val = COST_INF;
            int jmin = 0;
            for (int k = 0; k < n; k++) {
                int a = cost[i][k] - v[k];
                if (a < min_val || (a == min_val && fb[k] < 0)) {
                    min_val = a; jmin = k;
                }
            }
            u[i] = min_val;
            if (fb[jmin] >= 0) {
                bool done = false;
                for (int j = jmin; j < n && !done; j++) {
                    if (cost[i][j] - v[j] <= min_val) {
                        int rr = fb[j];
                        int kk = p[rr];
                        for (int k = kk; k < n && !done; k++) {
                            if (fb[k] < 0 && cost[rr][k] - u[rr] - v[k] == 0) {
                                f[rr] = k; fb[k] = rr; p[rr] = k + 1;
                                m++; f[i] = j; fb[j] = i; p[i] = j + 1;
                                done = true;
                            }
                        }
                        if (!done) p[rr] = n;
                    }
                }
            } else {
                m++; f[i] = jmin; fb[jmin] = i; p[i] = jmin + 1;
            }
        }
    }

    if (m != n) {
        for (int i = 0; i < n; i++) {
            if (f[i] < 0) {
                lr[0] = i;
                for (int k = 0; k < n; k++) {
                    pi[k] = cost[i][k] - u[i] - v[k];
                    rc[k] = i;
                    uc[k] = k;
                }
                int nuc = n, nlr = 1;
                int jj = -1;
                bool found = false;

                while (!found) {
                    for (int l = 0; l < nuc; l++) {
                        int j = uc[l];
                        if (pi[j] == 0) {
                            if (fb[j] < 0) { jj = j; found = true; break; }
                            lr[nlr++] = fb[j];
                            uc[l] = uc[nuc - 1]; nuc--;
                            int rr = fb[j];
                            for (int ll = 0; ll < nuc; ll++) {
                                int jjj = uc[ll];
                                int a = cost[rr][jjj] - u[rr] - v[jjj];
                                if (a < pi[jjj]) { pi[jjj] = a; rc[jjj] = rr; }
                            }
                            l = -1;
                        }
                    }
                    if (found) break;

                    int min_val = COST_INF;
                    for (int l = 0; l < nuc; l++) {
                        if (pi[uc[l]] < min_val) min_val = pi[uc[l]];
                    }
                    for (int l = 0; l < nlr; l++) u[lr[l]] += min_val;
                    for (int j = 0; j < n; j++) {
                        if (pi[j] == 0) v[j] -= min_val;
                        else pi[j] -= min_val;
                    }
                }

                while (jj >= 0) {
                    int ii = rc[jj];
                    fb[jj] = ii;
                    int prev = f[ii];
                    f[ii] = jj;
                    jj = prev;
                }
            }
        }
    }

    int total = 0;
    for (int i = 0; i < n; i++) {
        assignment[i] = f[i];
        if (f[i] >= 0 && f[i] < n) total += cost[i][f[i]];
    }

    free(u); free(v); free(fb); free(f);
    free(rc); free(pi); free(lr); free(uc); free(p);

    return total;
}

int bap_solve(int n, int **cost, int *assignment) {
    if (n <= 0) return 0;

    int lo = 0, hi = 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (cost[i][j] > hi) hi = cost[i][j];

    int *best_assign = (int *)calloc((size_t)n, sizeof(int));
    int best_bottleneck = hi + 1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;

        int **filtered = alloc_matrix_int(n, n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                filtered[i][j] = (cost[i][j] <= mid) ? 0 : COST_INF;

        int *temp_assign = (int *)calloc((size_t)n, sizeof(int));
        int total = lap_solve_hungarian(n, filtered, temp_assign);

        bool feasible = true;
        for (int i = 0; i < n; i++) {
            if (temp_assign[i] < 0 || temp_assign[i] >= n ||
                cost[i][temp_assign[i]] > mid) {
                feasible = false;
                break;
            }
        }

        if (feasible) {
            best_bottleneck = mid;
            memcpy(best_assign, temp_assign, (size_t)n * sizeof(int));
            hi = mid - 1;
        } else {
            lo = mid + 1;
        }

        free(temp_assign);
        free_matrix_int(filtered, n);
    }

    memcpy(assignment, best_assign, (size_t)n * sizeof(int));
    free(best_assign);
    return best_bottleneck;
}
