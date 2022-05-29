#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "options.h"

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

static void options_init_defaults(struct options *options)
{
  assert(options != NULL);

  options->mode = MODE_RAW;
  options->delay_ms = 100;
}

void print_usage(char **argv)
{
  printf(
      "piubtn-tester tool, build " __DATE__ " " __TIME__ " gitrev %s\n",
      STRINGIFY(GITREV));
  printf("Usage: %s [OPTION] ...\n", argv[0]);
  printf(
      "  -h  Print this help/usage message\n"
      "  -m  Mode (default: raw)\n"
      "        raw: Output raw output and input data, parsable for piping to "
      "other tooling\n"
      "        text: Human readable text output of output and input data\n"
      "        tui: Text user interface visualizing input and output, control "
      "outputs\n"
      "        bench: Benchmark the update call driving IO. Useful to debug "
      "performance/hardware issues\n"
      "  -d  Update loop delay in ms, use to reduce CPU load (default: 100)\n");
}

bool parse_args(struct options *options, int argc, char **argv)
{
  assert(options != NULL);
  assert(argv != NULL);

  options_init_defaults(options);

  for (int32_t i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-h")) {
      return false;
    } else if (!strcmp(argv[i], "-m")) {
      if (i + 1 >= argc) {
        fprintf(stderr, "Missing parameter for -m argument\n");
        return false;
      }

      i++;

      if (!strcmp(argv[i], "tui")) {
        options->mode = MODE_TUI;
      } else if (!strcmp(argv[i], "raw")) {
        options->mode = MODE_RAW;
      } else if (!strcmp(argv[i], "text")) {
        options->mode = MODE_TEXT;
      } else if (!strcmp(argv[i], "bench")) {
        options->mode = MODE_BENCHMARK;
      } else {
        fprintf(stderr, "Invalid parameter for -m argument\n");
        return false;
      }
    } else if (!strcmp(argv[i], "-d")) {
      int32_t tmp;

      if (i + 1 >= argc) {
        fprintf(stderr, "Missing parameter for -d argument\n");
        return false;
      }

      i++;

      tmp = atoi(argv[i]);

      if (tmp <= 0) {
        fprintf(stderr, "Invalid value for for -d argument, must be > 0\n");
        return false;
      }

      options->delay_ms = tmp;
    }
  }

  return true;
}