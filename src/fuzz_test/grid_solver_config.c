#include <stdlib.h>

#include "../lib/utils/max_min.h"
#include "./config.h"

static struct LinearSolver default_linear_solver = {
    .solve = linear_solve_aggarwal,
    .params = NULL,
    .name = "Aggarwal solver",
};

static const struct GridSolver *grid_solvers[] = {
    &(struct GridSolver){
        .solve = red_rec,
        .params = &(RedRecParams){.linear_solver = &default_linear_solver},
        .name = "Red Rec",
    },
};

static struct Grid *grid_generator(int width, int height) {
  return grid_factory.generate_compact_target_region(width, height,
                                                     rand() % max(height, 1));
}

struct GridSolversFuzzTestConfig grid_solvers_config = {
    .width_range = {0, 35},
    .height_range = {0, 35},
    .grid_solvers = grid_solvers,
    .grid_solvers_num = sizeof(grid_solvers) / sizeof(grid_solvers[0]),
    .time_limit_in_seconds = 5.0,
    .grid_generator = grid_generator,
};
