#pragma once

struct Range {
  int start;
  int exclusive_end;
};

struct Range get_range(int range_index, int range_num, int total_length);
int get_range_index(int index, int range_num, int total_length);
