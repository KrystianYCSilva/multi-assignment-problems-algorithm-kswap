#include "result_writer.h"
#include <stdio.h>

int result_write_csv(const char *filepath, const char *instance_name,
                     const char *constraint_name, int iterations,
                     int initial_cost, const Schedule *s, double elapsed_sec) {
    FILE *fp = fopen(filepath, "a");
    if (!fp) {
        fprintf(stderr, "Error: cannot open result file '%s'\n", filepath);
        return -1;
    }
    fprintf(fp, "%s;%s;%d;%d;%d;%d;%d;%f\n",
            instance_name, constraint_name, iterations,
            initial_cost, s->cost, s->hard_violations, s->soft_violations,
            elapsed_sec);
    fclose(fp);
    return 0;
}
