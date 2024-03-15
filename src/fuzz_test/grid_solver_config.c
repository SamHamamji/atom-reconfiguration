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
    &(struct GridSolver){
        .solve = red_rec_deferred_solving,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 3,
            },
        .name = "Red Rec deferred solving",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 1,
            },
        .name = "Red Rec parallel (1 thread)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 7,
            },
        .name = "Red Rec parallel (7 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_single_consumer,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 1,
            },
        .name = "Red Rec parallel single consumer (1 thread)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_single_consumer,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 7,
            },
        .name = "Red Rec parallel single consumer (7 threads)",
    },
};

static struct Grid *grid_generator(int width, int height) {
  return grid_factory.generate_compact_target_region(width, height,
                                                     rand() % max(height, 1));
}

struct GridSolversFuzzTestConfig grid_solvers_config = {
    .width_range = {0, 500},
    .height_range = {0, 500},
    .grid_solvers = grid_solvers,
    .grid_solvers_num = sizeof(grid_solvers) / sizeof(grid_solvers[0]),
    .time_limit_in_seconds = 5.0,
    .grid_generator = grid_generator,
};
