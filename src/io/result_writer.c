#include "result_writer.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static void mkdirs(const char *path) {
    char tmp[512];
    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            *p = '\0';
            mkdir(tmp);
            *p = '/';
        }
    }
    mkdir(tmp);
}

static void ensure_parent_dir(const char *filepath) {
    char dir[512];
    strncpy(dir, filepath, sizeof(dir) - 1);
    dir[sizeof(dir) - 1] = '\0';
    char *last_sep = NULL;
    for (char *p = dir; *p; p++) {
        if (*p == '/' || *p == '\\') last_sep = p;
    }
    if (last_sep) {
        *last_sep = '\0';
        mkdirs(dir);
    }
}

int result_write_csv(const char *filepath, const char *instance_name,
                     const char *constraint_name, int iterations,
                     int initial_cost, const Schedule *s, double elapsed_sec) {
    ensure_parent_dir(filepath);
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
