#ifndef MAPA_RESULT_WRITER_H
#define MAPA_RESULT_WRITER_H

#include "../core/types.h"

int result_write_csv(const char *filepath, const char *instance_name,
                     const char *constraint_name, int iterations,
                     int initial_cost, const Schedule *s, double elapsed_sec);

#endif
