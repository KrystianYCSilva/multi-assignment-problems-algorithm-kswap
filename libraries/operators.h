#include<stdio.h>
#include<stdlib.h>
#include "cost_matrix.h"

Schedule* pcr(Schedule* s, NspLib* nsp, Constraints* c);
Schedule* prt(Schedule* s, NspLib* nsp, Constraints* c);
Schedule* pcr_backward(Schedule* s, NspLib* nsp, Constraints* c);
Schedule* prt_backward(Schedule* s, NspLib* nsp, Constraints* c);
Schedule* kSwap(Schedule* s, NspLib* nsp, Constraints* c, int k);
Schedule* kSwap_backward(Schedule* s, NspLib* nsp, Constraints * c, int k);
