#ifndef MAPA_NSP_READER_H
#define MAPA_NSP_READER_H

#include "../core/types.h"

NspInstance *nsp_read(const char *filepath);
void nsp_free(NspInstance *nsp);
void nsp_print(const NspInstance *nsp);

Constraints *constraints_read(const char *filepath);
void constraints_free(Constraints *c);
void constraints_print(const Constraints *c);

#endif
