#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct StackElement {
  struct StackElement *next;
  int value;
};

struct StackElement *stack_pop(struct StackElement *stack_head);
struct StackElement *stack_push(struct StackElement *stack_head, int value);
void stack_copy(struct StackElement *stack, int *array, int value);
bool stack_is_empty(const struct StackElement *stack_head);
void stack_print(struct StackElement *stack_head);
void stack_free(struct StackElement *stack_element);
