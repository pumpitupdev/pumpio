#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "piubtn-usb.h"
#include "piubtn.h"

#include "options.h"

typedef bool (*func_render_data_t)(
    union piubtn_output_paket *output,
    const union piubtn_input_paket *input,
    double io_time_sec);

static bool interrupted = false;

// -----------------------------------------------------------------------------------------

static void sig_handler(int sig)
{
  interrupted = true;
}

static void sleep_ms(int32_t time_ms)
{
  usleep(time_ms * 1000);
}

// -----------------------------------------------------------------------------------------

static void handle_button_lights_on_input(
    struct piubtn_piu_output_paket *output,
    const struct piubtn_piu_input_paket *input)
{
    output->btn_light_p1_start = input->btn_p1_start;
    output->btn_light_p1_right = input->btn_p1_right;
    output->btn_light_p1_left = input->btn_p1_left;
    output->btn_light_p1_back = input->btn_p1_back;

    output->btn_light_p2_start = input->btn_p2_start;
    output->btn_light_p2_right = input->btn_p2_right;
    output->btn_light_p2_left = input->btn_p2_left;
    output->btn_light_p2_back = input->btn_p2_back;
}

static bool draw_menu_tui(struct piubtn_piu_output_paket *output)
{
  int state;

  system("clear");
  printf(
      "Menu options:\n"
      "  0: Exit menu and continue loop\n"
      "  1: Exit\n"
      "  2: All lamps on\n"
      "  3: All lamps off)\n"
      "Waiting for input (confirm with enter): ");

  scanf("%d", &state);

  switch (state) {
    default:
      // fallthrough
    case 0:
      // noop
      break;

    case 1:
      return false;

    case 2:
        output->btn_light_p1_start = 1;
        output->btn_light_p1_right = 1;
        output->btn_light_p1_left = 1;
        output->btn_light_p1_back = 1;

        output->btn_light_p2_start = 1;
        output->btn_light_p2_right = 1;
        output->btn_light_p2_left = 1;
        output->btn_light_p2_back = 1;

      break;

    case 3:
        output->btn_light_p1_start = 0;
        output->btn_light_p1_right = 0;
        output->btn_light_p1_left = 0;
        output->btn_light_p1_back = 0;

        output->btn_light_p2_start = 0;
        output->btn_light_p2_right = 0;
        output->btn_light_p2_left = 0;
        output->btn_light_p2_back = 0;

      break;
  }

  return true;
}

static void draw_tui(
    struct piubtn_piu_output_paket *output,
    const struct piubtn_piu_input_paket *input,
    double io_time_sec)
{
  static uint8_t counter = 0;

  assert(output != NULL);
  assert(input != NULL);

  system("clear");
  printf("Press CTRL + C to open menu\n");
  printf("%d - I/O time: %.5f secs\n", counter++, io_time_sec);
  printf(
    "+--------------------+\n"
    "|    %d          %d    |\n"
    "|  +---+      +---+  |\n"
    "|  | %d |      | %d |  |\n"
    "| %d|%d+%d|%d    %d|%d+%d|%d |\n"
    "|  | %d |      | %d |  |\n"
    "|  +---+      +---+  |\n"
    "|    %d          %d    |\n"
    "+--------------------+\n",
      input->btn_p1_back,
      input->btn_p2_back,
      output->btn_light_p1_back,
      output->btn_light_p1_back,
      input->btn_p1_left,
      output->btn_light_p1_left,
      output->btn_light_p1_right,
      input->btn_p1_right,
      input->btn_p2_left,
      output->btn_light_p2_left,
      output->btn_light_p2_right,
      input->btn_p2_right,
      output->btn_light_p1_start,
      output->btn_light_p2_start,
      input->btn_p1_start,
      input->btn_p2_start);
}

// -----------------------------------------------------------------------------------------

static bool render_raw(
    union piubtn_output_paket *output,
    const union piubtn_input_paket *input,
    double io_time_sec)
{
  assert(output != NULL);
  assert(input != NULL);

  for (size_t i = 0; i < sizeof(output->raw); i++) {
    printf("%02X ", output->raw[i]);
  }

  for (size_t i = 0; i < sizeof(input->raw); i++) {
    printf("%02X ", input->raw[i]);
  }

  printf("\n");

  handle_button_lights_on_input(&output->piu, &input->piu);

  return !interrupted;
}

static bool render_text(
    union piubtn_output_paket *output,
    const union piubtn_input_paket *input,
    double io_time_sec)
{
  assert(output != NULL);
  assert(input != NULL);

  static uint8_t counter = 0;

  system("clear");
  printf(
      "%d - I/O time: %.5f secs\n"
      "p1_back: %d\n"
      "p1_left: %d\n"
      "p1_right: %d\n"
      "p1_start: %d\n"
      "p2_back: %d\n"
      "p2_left: %d\n"
      "p2_right: %d\n"
      "p2_start: %d\n",
      counter++,
      io_time_sec,
     input->piu.btn_p1_back,
     input->piu.btn_p1_left,
     input->piu.btn_p1_right,
     input->piu.btn_p1_start,
     input->piu.btn_p2_back,
     input->piu.btn_p2_left,
     input->piu.btn_p2_right,
     input->piu.btn_p2_start);

  handle_button_lights_on_input(&output->piu, &input->piu);

  return !interrupted;
}

static bool render_tui(
    union piubtn_output_paket *output,
    const union piubtn_input_paket *input,
    double io_time_sec)
{
  assert(output != NULL);
  assert(input != NULL);

  if (interrupted) {
    if (draw_menu_tui(&output->piu)) {
      interrupted = false;
    } else {
      return false;
    }
  }

  draw_tui(&output->piu,&input->piu,io_time_sec);

  handle_button_lights_on_input(&output->piu, &input->piu);

  return true;
}

static bool render_benchmark(
    union piubtn_output_paket *output,
    const union piubtn_input_paket *input,
    double io_time_sec)
{
  assert(output);
  assert(input);

  static uint64_t counter = 0;
  static double accu = 0;

  counter++;
  accu += io_time_sec;

  system("clear");
  printf("Samples: %ld\n", counter);
  printf("Latest time: %.5f secs\n", io_time_sec);
  printf("Average time: %.5f secs\n", accu / counter);

  return !interrupted;
}

// -----------------------------------------------------------------------------------------

static void proc_usb(int32_t delay_ms, func_render_data_t render)
{
  void *handle;
  int32_t result;
  union piubtn_output_paket output;
  union piubtn_input_paket input;
  struct timespec tstart;
  struct timespec tend;
  double io_time_sec;
  bool loop;

  assert(render);

  memset(&tstart, 0, sizeof(struct timespec));
  memset(&tend, 0, sizeof(struct timespec));

  memset(&output, 0, sizeof(output.raw));
  memset(&input, 0, sizeof(input.raw));

  result = piubtn_usb_open(&handle);

  if (result) {
    errno = result;
    perror("Opening PIUBTN failed");
    exit(EXIT_FAILURE);
  }

  loop = true;

  while (loop) {
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    result = piubtn_usb_poll(handle, &output, &input);

    clock_gettime(CLOCK_MONOTONIC, &tend);

    io_time_sec = ((double) tend.tv_sec + 1.0e-9 * tend.tv_nsec) -
        ((double) tstart.tv_sec + 1.0e-9 * tstart.tv_nsec);

    if (result) {
      errno = result;
      perror("Running update cycle for PIUBTN failed");
      exit(EXIT_FAILURE);
    }

    loop = render(&output, &input, io_time_sec);

    sleep_ms(delay_ms);
  }

  piubtn_usb_close(handle);
}

// -----------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  struct options options;

  signal(SIGINT, sig_handler);

  if (!parse_args(&options, argc, argv)) {
    print_usage(argv);
    return EXIT_FAILURE;
  }

  if (options.mode == MODE_RAW) {
    proc_usb(options.delay_ms, render_raw);
  } else if (options.mode == MODE_TEXT) {
    proc_usb(options.delay_ms, render_text);
  } else if (options.mode == MODE_TUI) {
    proc_usb(options.delay_ms, render_tui);
  } else if (options.mode == MODE_BENCHMARK) {
    proc_usb(options.delay_ms, render_benchmark);
  } else {
    fprintf(stderr, "Invalid parameters selected\n");
    print_usage(argv);
  }

  return EXIT_SUCCESS;
}
