#include "./red_rec_utils.h"

void execute_move(struct Grid *grid, struct Reconfiguration *reconfiguration,
                  struct Range *fixed_sources_range,
                  struct ColumnPair column_pair) {
  struct Point *donor = grid_get_column(grid, column_pair.donor_index);

  for (int bool_value = 0; bool_value < 2; bool_value++) {
    bool moving_upper_sources = (bool)bool_value;
    int target_row = moving_upper_sources ? fixed_sources_range->start - 1
                                          : fixed_sources_range->exclusive_end;

    for (int i = target_row; 0 <= i && i < grid->height;
         i += moving_upper_sources ? -1 : 1) {
      if (!donor[target_row].is_target) {
        break;
      }

      if (!donor[i].is_source ||
          (donor[i].is_target &&
           column_pair.donor_index != column_pair.receiver_index)) {
        continue;
      }

      struct Move horizontal_move = {
          .origin = {.col = column_pair.donor_index, .row = i},
          .destination = {.col = column_pair.receiver_index, .row = i},
      };
      struct Move vertical_move = {
          .origin = {.col = column_pair.receiver_index, .row = i},
          .destination = {.col = column_pair.receiver_index, .row = target_row},
      };

      reconfiguration_add_move(reconfiguration, horizontal_move);
      reconfiguration_add_move(reconfiguration, vertical_move);

      grid_apply_move(grid, horizontal_move);
      grid_apply_move(grid, vertical_move);

      if (moving_upper_sources) {
        fixed_sources_range->start--;
        target_row--;
      } else {
        fixed_sources_range->exclusive_end++;
        target_row++;
      }
    }
  }
}
