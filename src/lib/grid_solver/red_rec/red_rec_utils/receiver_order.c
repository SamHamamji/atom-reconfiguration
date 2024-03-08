#include <stdlib.h>

#include "./red_rec_utils.h"

struct ReceiverOrder *receiver_order_new(int max_receiver_num) {
  struct ReceiverOrder *receiver_order = malloc(sizeof(struct ReceiverOrder));
  *receiver_order = (struct ReceiverOrder){
      .receiver_indexes = malloc(max_receiver_num * sizeof(int)),
      .receiver_num = 0,
  };

  return receiver_order;
}

void receiver_order_free(struct ReceiverOrder *receiver_order) {
  free(receiver_order->receiver_indexes);
  free(receiver_order);
}

void receiver_order_push(struct ReceiverOrder *receiver_order,
                         int receiver_index) {
  receiver_order->receiver_indexes[receiver_order->receiver_num] =
      receiver_index;
  receiver_order->receiver_num++;
}
