#include "nsp_reader.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 100000

static int parse_int(const char *s, int *pos) {
    while (s[*pos] && !isdigit((unsigned char)s[*pos]) && s[*pos] != '-') (*pos)++;
    if (!s[*pos]) return 0;
    int sign = 1;
    if (s[*pos] == '-') { sign = -1; (*pos)++; }
    int val = 0;
    while (isdigit((unsigned char)s[*pos])) {
        val = val * 10 + (s[*pos] - '0');
        (*pos)++;
    }
    return sign * val;
}

static int *parse_line_ints(const char *line, int expected, int *actual) {
    int cap = (expected > 0) ? expected : 64;
    int *arr = (int *)calloc((size_t)cap, sizeof(int));
    int count = 0, pos = 0;
    while (line[pos]) {
        while (line[pos] && (line[pos] == '\t' || line[pos] == ' ' || line[pos] == '\r' || line[pos] == '\n')) pos++;
        if (!line[pos]) break;
        if (!isdigit((unsigned char)line[pos]) && line[pos] != '-') { pos++; continue; }
        arr[count++] = parse_int(line, &pos);
        if (count >= cap) {
            cap *= 2;
            arr = (int *)realloc(arr, (size_t)cap * sizeof(int));
        }
    }
    *actual = count;
    return arr;
}

NspInstance *nsp_read(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open NSP file '%s'\n", filepath);
        return NULL;
    }

    NspInstance *nsp = (NspInstance *)calloc(1, sizeof(NspInstance));
    char line[MAX_LINE];
    int n_line = 0;
    int idx_coverage = 0, idx_pref = 0;

    while (fgets(line, MAX_LINE, f)) {
        if (n_line == 0) {
            int count = 0;
            int *vals = parse_line_ints(line, 3, &count);
            nsp->size.n_nurses = vals[0];
            nsp->size.n_days = vals[1];
            nsp->size.n_shifts = vals[2];
            free(vals);

            nsp->coverage_matrix = alloc_matrix_int(nsp->size.n_days, nsp->size.n_shifts);
            nsp->preference_matrix = alloc_matrix_int(nsp->size.n_nurses,
                                                       nsp->size.n_days * nsp->size.n_shifts);
        } else if (n_line >= 2 && idx_coverage < nsp->size.n_days) {
            int count = 0;
            int *vals = parse_line_ints(line, nsp->size.n_shifts, &count);
            for (int j = 0; j < nsp->size.n_shifts && j < count; j++)
                nsp->coverage_matrix[idx_coverage][j] = vals[j];
            free(vals);
            idx_coverage++;
        } else if (n_line >= nsp->size.n_days + 3 && idx_pref < nsp->size.n_nurses) {
            int count = 0;
            int cols = nsp->size.n_days * nsp->size.n_shifts;
            int *vals = parse_line_ints(line, cols, &count);
            for (int j = 0; j < cols && j < count; j++)
                nsp->preference_matrix[idx_pref][j] = vals[j];
            free(vals);
            idx_pref++;
        }
        n_line++;
    }

    fclose(f);
    return nsp;
}

void nsp_free(NspInstance *nsp) {
    if (!nsp) return;
    free_matrix_int(nsp->coverage_matrix, nsp->size.n_days);
    free_matrix_int(nsp->preference_matrix, nsp->size.n_nurses);
    free(nsp);
}

void nsp_print(const NspInstance *nsp) {
    printf("NSP Instance: %d nurses, %d days, %d shifts\n",
           nsp->size.n_nurses, nsp->size.n_days, nsp->size.n_shifts);
    printf("Coverage:\n");
    for (int d = 0; d < nsp->size.n_days; d++) {
        printf("  Day %d: ", d);
        for (int s = 0; s < nsp->size.n_shifts; s++)
            printf("%d ", nsp->coverage_matrix[d][s]);
        printf("\n");
    }
}

Constraints *constraints_read(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open constraints file '%s'\n", filepath);
        return NULL;
    }

    Constraints *c = (Constraints *)calloc(1, sizeof(Constraints));
    char line[MAX_LINE];
    int n_line = 0;
    int idx_matrix = 0;

    while (fgets(line, MAX_LINE, f)) {
        int count = 0;
        int *vals = NULL;

        if (n_line == 0) {
            vals = parse_line_ints(line, 2, &count);
            c->size.n_shifts = vals[0];
            c->size.n_days = (count > 1) ? vals[1] : 0;
            free(vals);
            c->consecutive_assignments_matrix = alloc_matrix_int(c->size.n_shifts, 4);
        } else if (n_line == 2) {
            vals = parse_line_ints(line, 2, &count);
            c->number_of_assignments = (int *)calloc(2, sizeof(int));
            c->number_of_assignments[0] = vals[0];
            c->number_of_assignments[1] = (count > 1) ? vals[1] : vals[0];
            free(vals);
        } else if (n_line == 4) {
            vals = parse_line_ints(line, 2, &count);
            c->consecutive_working_shifts = (int *)calloc(2, sizeof(int));
            c->consecutive_working_shifts[0] = vals[0];
            c->consecutive_working_shifts[1] = (count > 1) ? vals[1] : vals[0];
            free(vals);
        } else if (n_line > 6 && idx_matrix < c->size.n_shifts) {
            vals = parse_line_ints(line, 4, &count);
            for (int j = 0; j < 4 && j < count; j++)
                c->consecutive_assignments_matrix[idx_matrix][j] = vals[j];
            free(vals);
            idx_matrix++;
        }
        n_line++;
    }

    fclose(f);
    return c;
}

void constraints_free(Constraints *c) {
    if (!c) return;
    free(c->number_of_assignments);
    free(c->consecutive_working_shifts);
    free_matrix_int(c->consecutive_assignments_matrix, c->size.n_shifts);
    free(c);
}

void constraints_print(const Constraints *c) {
    printf("Constraints: shifts=%d\n", c->size.n_shifts);
    printf("  Assignments: [%d, %d]\n", c->number_of_assignments[0], c->number_of_assignments[1]);
    printf("  Consecutive work: [%d, %d]\n", c->consecutive_working_shifts[0], c->consecutive_working_shifts[1]);
    for (int i = 0; i < c->size.n_shifts; i++)
        printf("  Shift %d: consec=[%d,%d] assign=[%d,%d]\n", i,
               c->consecutive_assignments_matrix[i][0], c->consecutive_assignments_matrix[i][1],
               c->consecutive_assignments_matrix[i][2], c->consecutive_assignments_matrix[i][3]);
}
