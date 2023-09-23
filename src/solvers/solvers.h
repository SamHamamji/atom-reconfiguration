#include "../interval/interval.h"
#include "../mapping/mapping.h"

typedef struct Mapping *Solver(const struct Interval *interval);

Solver iterative_solver;
Solver karp_li_solver;
