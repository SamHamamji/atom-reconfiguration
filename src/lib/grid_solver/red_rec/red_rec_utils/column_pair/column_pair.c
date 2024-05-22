#include <assert.h>

#include "../../../../utils/max_min.h"
#include "../red_rec_utils.h"

int get_exchange_num(struct ColumnPair column_pair) {
  assert(column_pair.donor_surplus >= 0);
  assert(column_pair.receiver_deficit <= 0);

  return min(column_pair.donor_surplus, -column_pair.receiver_deficit);
}
