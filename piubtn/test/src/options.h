#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

enum mode {
  MODE_RAW = 0,
  MODE_TEXT = 1,
  MODE_TUI = 2,
  MODE_BENCHMARK = 3,
};

struct options {
  enum mode mode;
  uint32_t delay_ms;
};

void print_usage(char **argv);
bool parse_args(struct options *options, int argc, char **argv);

#endif