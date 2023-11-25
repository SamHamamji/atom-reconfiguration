#include <stdbool.h>
#include <stdlib.h>

#include "stack.h"

struct StackElement *stack_pop(struct StackElement *stack_head) {
  struct StackElement *new_stack_head = stack_head->next;
  free(stack_head);
  return new_stack_head;
}

struct StackElement *stack_push(struct StackElement *stack_head, int value) {
  struct StackElement *new_stack = malloc(sizeof(struct StackElement));
  new_stack->next = stack_head;
  new_stack->value = value;
  return new_stack;
}

void stack_copy(struct StackElement *stack, int *array, int value) {
  while (!stack_is_empty(stack)) {
    array[stack->value] = value;
    stack = stack->next;
  }
}

inline bool stack_is_empty(const struct StackElement *stack_head) {
  return stack_head == NULL;
}

void stack_free(struct StackElement *stack_element) {
  if (stack_is_empty(stack_element)) {
    return;
  }
  stack_free(stack_element->next);
  free(stack_element);
}
