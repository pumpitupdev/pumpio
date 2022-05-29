#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

enum game {
  GAME_PIU = 0,
  GAME_ITG = 1,
};

enum mode {
  MODE_RAW = 0,
  MODE_TEXT = 1,
  MODE_TUI = 2,
  MODE_BENCHMARK = 3,
};

enum type {
  TYPE_USB = 0,
  TYPE_KMOD = 1,
};

struct options {
  enum game game;
  enum mode mode;
  enum type type;
  uint32_t delay_ms;
};

void print_usage(char **argv);
bool parse_args(struct options *options, int argc, char **argv);

#endif