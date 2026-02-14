#include "cost.h"
#include <stdio.h>

#include "schedule.h"

bool is_forbidden_sequence(int prev_shift, int next_shift) {
    if (prev_shift == SHIFT_EVENING && next_shift == SHIFT_MORNING) return true;
    if (prev_shift == SHIFT_NIGHT && next_shift == SHIFT_MORNING) return true;
    if (prev_shift == SHIFT_NIGHT && next_shift == SHIFT_EVENING) return true;
    return false;
}

int count_consecutive_work(const Schedule *s, int nurse) {
    int max_run = 0, run = 0;
    for (int d = 0; d < s->size.n_days; d++) {
        if (s->nurse_schedule[nurse][d] != SHIFT_FREE) {
            run++;
            if (run > max_run) max_run = run;
        } else {
            run = 0;
        }
    }
    return max_run;
}

int count_consecutive_same_shift(const Schedule *s, int nurse, int shift) {
    int max_run = 0, run = 0;
    for (int d = 0; d < s->size.n_days; d++) {
        if (s->nurse_schedule[nurse][d] == shift) {
            run++;
            if (run > max_run) max_run = run;
        } else {
            run = 0;
        }
    }
    return max_run;
}

int count_total_assignments(const Schedule *s, int nurse) {
    int count = 0;
    for (int d = 0; d < s->size.n_days; d++)
        if (s->nurse_schedule[nurse][d] != SHIFT_FREE)
            count++;
    return count;
}

static int count_forbidden_sequences(const Schedule *s, int nurse) {
    int count = 0;
    for (int d = 0; d < s->size.n_days - 1; d++) {
        if (is_forbidden_sequence(s->nurse_schedule[nurse][d],
                                  s->nurse_schedule[nurse][d + 1]))
            count++;
    }
    return count;
}

int cost_partial_nurse(const Schedule *s, const Constraints *c,
                       const NspInstance *nsp, int nurse) {
    int n_days = s->size.n_days;
    int n_shifts = s->size.n_shifts;
    int hcv = 0, scv = 0, pref_cost = 0;

    for (int d = 0; d < n_days; d++) {
        int sh = s->nurse_schedule[nurse][d];
        pref_cost += nsp->preference_matrix[nurse][d * n_shifts + sh];
    }

    hcv += count_forbidden_sequences(s, nurse);

    int total_assign = count_total_assignments(s, nurse);
    if (total_assign < c->number_of_assignments[0] ||
        total_assign > c->number_of_assignments[1])
        hcv++;

    int consec_work = count_consecutive_work(s, nurse);
    if (consec_work < c->consecutive_working_shifts[0] ||
        consec_work > c->consecutive_working_shifts[1])
        scv++;

    for (int sh = 0; sh < NUM_WORK_SHIFTS; sh++) {
        int consec_same = count_consecutive_same_shift(s, nurse, sh);
        int min_consec = c->consecutive_assignments_matrix[sh][0];
        int max_consec = c->consecutive_assignments_matrix[sh][1];
        if (consec_same > 0 && (consec_same < min_consec || consec_same > max_consec))
            scv++;

        int total_sh = 0;
        for (int d = 0; d < n_days; d++)
            if (s->nurse_schedule[nurse][d] == sh)
                total_sh++;
        int min_assign = c->consecutive_assignments_matrix[sh][2];
        int max_assign = c->consecutive_assignments_matrix[sh][3];
        if (total_sh > 0 && (total_sh < min_assign || total_sh > max_assign))
            scv++;
    }

    return pref_cost + PENALTY_HARD * hcv + PENALTY_SOFT * scv;
}

static int evaluate_internal(Schedule *s, const Constraints *c,
                             const NspInstance *nsp, bool verbose) {
    int n_nurses = s->size.n_nurses;
    int n_days = s->size.n_days;
    int n_shifts = s->size.n_shifts;
    int total_cost = 0;
    int total_hcv = 0, total_scv = 0;

    for (int nurse = 0; nurse < n_nurses; nurse++) {
        int hcv = 0, scv = 0, pref_cost = 0;

        for (int d = 0; d < n_days; d++) {
            int sh = s->nurse_schedule[nurse][d];
            pref_cost += nsp->preference_matrix[nurse][d * n_shifts + sh];
        }

        hcv += count_forbidden_sequences(s, nurse);

        int total_assign = count_total_assignments(s, nurse);
        if (total_assign < c->number_of_assignments[0] ||
            total_assign > c->number_of_assignments[1])
            hcv++;

        int consec_work = count_consecutive_work(s, nurse);
        if (consec_work < c->consecutive_working_shifts[0] ||
            consec_work > c->consecutive_working_shifts[1])
            scv++;

        for (int sh = 0; sh < NUM_WORK_SHIFTS; sh++) {
            int consec_same = count_consecutive_same_shift(s, nurse, sh);
            int min_c_same = c->consecutive_assignments_matrix[sh][0];
            int max_c_same = c->consecutive_assignments_matrix[sh][1];
            if (consec_same > 0 && (consec_same < min_c_same || consec_same > max_c_same))
                scv++;

            int total_sh = 0;
            for (int d = 0; d < n_days; d++)
                if (s->nurse_schedule[nurse][d] == sh) total_sh++;
            int min_a = c->consecutive_assignments_matrix[sh][2];
            int max_a = c->consecutive_assignments_matrix[sh][3];
            if (total_sh > 0 && (total_sh < min_a || total_sh > max_a))
                scv++;
        }

        total_hcv += hcv;
        total_scv += scv;
        total_cost += pref_cost + PENALTY_HARD * hcv + PENALTY_SOFT * scv;
    }

    for (int d = 0; d < n_days; d++) {
        for (int sh = 0; sh < NUM_WORK_SHIFTS; sh++) {
            int count = schedule_count_shift_on_day(s, d, sh);
            if (count < nsp->coverage_matrix[d][sh]) {
                total_scv += (nsp->coverage_matrix[d][sh] - count);
                total_cost += PENALTY_SOFT * (nsp->coverage_matrix[d][sh] - count);
            }
        }
    }

    s->cost = total_cost;
    s->hard_violations = total_hcv;
    s->soft_violations = total_scv;

    if (verbose) {
        printf("Cost=%d HCV=%d SCV=%d\n", total_cost, total_hcv, total_scv);
    }

    return total_cost;
}

int cost_evaluate(Schedule *s, const Constraints *c, const NspInstance *nsp) {
    return evaluate_internal(s, c, nsp, true);
}

int cost_evaluate_silent(Schedule *s, const Constraints *c, const NspInstance *nsp) {
    return evaluate_internal(s, c, nsp, false);
}
