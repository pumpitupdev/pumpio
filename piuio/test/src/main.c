#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "piuio-kmod.h"
#include "piuio-usb.h"
#include "piuio.h"

#include "options.h"

typedef bool (*func_render_data_t)(
    union piuio_output_paket *output,
    const struct piuio_usb_input_batch_paket *input,
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

static void handle_pad_lights_on_input_piu(
    struct piuio_piu_output_paket *output,
    const struct piuio_piu_input_paket *inputs)
{
  for (uint8_t i = 0; i < PIUIO_SENSOR_MASK_TOTAL_COUNT; i++) {
    output->pad_light_p1_lu |= inputs[i].sensor_p1_lu;
    output->pad_light_p1_ru |= inputs[i].sensor_p1_ru;
    output->pad_light_p1_cn |= inputs[i].sensor_p1_cn;
    output->pad_light_p1_ld |= inputs[i].sensor_p1_ld;
    output->pad_light_p1_rd |= inputs[i].sensor_p1_rd;

    output->pad_light_p2_lu |= inputs[i].sensor_p2_lu;
    output->pad_light_p2_ru |= inputs[i].sensor_p2_ru;
    output->pad_light_p2_cn |= inputs[i].sensor_p2_cn;
    output->pad_light_p2_ld |= inputs[i].sensor_p2_ld;
    output->pad_light_p2_rd |= inputs[i].sensor_p2_rd;
  }
}

static void handle_pad_lights_on_input_itg(
    struct piuio_itg_output_paket *output,
    const struct piuio_itg_input_paket *inputs)
{
  for (uint8_t i = 0; i < PIUIO_SENSOR_MASK_TOTAL_COUNT; i++) {
    output->pad_light_p1_up |= inputs[i].sensor_p1_up;
    output->pad_light_p1_down |= inputs[i].sensor_p1_down;
    output->pad_light_p1_left |= inputs[i].sensor_p1_left;
    output->pad_light_p1_right |= inputs[i].sensor_p1_right;

    output->pad_light_p2_up |= inputs[i].sensor_p2_up;
    output->pad_light_p2_down |= inputs[i].sensor_p2_down;
    output->pad_light_p2_left |= inputs[i].sensor_p2_left;
    output->pad_light_p2_right |= inputs[i].sensor_p2_right;
  }
}

static bool draw_menu_piu_tui(struct piuio_piu_output_paket *output)
{
  int n;
  int state;
  char buf[5];

  system("clear");
  printf(
      "Menu options:\n"
      "  0: Exit menu and continue loop\n"
      "  1: Exit\n"
      "  2: Set bass lights state\n"
      "  3: Set top lamp state\n"
      "  4: All lamps on\n"
      "  5: All lamps off)\n"
      "Waiting for input (confirm with enter): ");

  n = scanf("%d", &state);

  switch (state) {
    default:
      // fallthrough
    case 0:
      // noop
      break;

    case 1:
      return false;

    case 2:
      printf("Enter bass lights state (0/1): ");
      n = scanf("%d", &state);

      if (n > 0) {
        output->light_bass = state > 0;
      }

      break;

    case 3:
      printf("Enter top lamp state, chain of 4x 0/1s: ");
      memset(buf, 0, sizeof(buf));
      n = scanf("%4s", buf);

      if (n > 0) {
        output->top_lamp_l1 = buf[0] == '1';
        output->top_lamp_l2 = buf[1] == '1';
        output->top_lamp_r1 = buf[2] == '1';
        output->top_lamp_r2 = buf[3] == '1';
      }

      break;

    case 4:
      output->light_bass = 1;

      output->top_lamp_l1 = 1;
      output->top_lamp_l2 = 1;
      output->top_lamp_r1 = 1;
      output->top_lamp_r2 = 1;

      output->pad_light_p1_lu = 1;
      output->pad_light_p1_ru = 1;
      output->pad_light_p1_cn = 1;
      output->pad_light_p1_ld = 1;
      output->pad_light_p1_rd = 1;

      output->pad_light_p2_lu = 1;
      output->pad_light_p2_ru = 1;
      output->pad_light_p2_cn = 1;
      output->pad_light_p2_ld = 1;
      output->pad_light_p2_rd = 1;

      break;

    case 5:
      output->light_bass = 0;

      output->top_lamp_l1 = 0;
      output->top_lamp_l2 = 0;
      output->top_lamp_r1 = 0;
      output->top_lamp_r2 = 0;

      output->pad_light_p1_lu = 0;
      output->pad_light_p1_ru = 0;
      output->pad_light_p1_cn = 0;
      output->pad_light_p1_ld = 0;
      output->pad_light_p1_rd = 0;

      output->pad_light_p2_lu = 0;
      output->pad_light_p2_ru = 0;
      output->pad_light_p2_cn = 0;
      output->pad_light_p2_ld = 0;
      output->pad_light_p2_rd = 0;

      break;
  }

  return true;
}

static bool draw_menu_itg_tui(struct piuio_itg_output_paket *output)
{
  int n;
  int state;
  char buf[5];

  system("clear");
  printf(
      "Menu options:\n"
      "  0: Exit menu and continue loop\n"
      "  1: Exit\n"
      "  2: Set bass lights state\n"
      "  3: Set top lamp state\n"
      "  4: All lamps on\n"
      "  5: All lamps off)\n"
      "Waiting for input (confirm with enter): ");

  n = scanf("%d", &state);

  switch (state) {
    default:
      // fallthrough
    case 0:
      // noop
      break;

    case 1:
      return false;

    case 2:
      printf("Enter bass lights state (0/1): ");
      n = scanf("%d", &state);

      if (n > 0) {
        output->light_bass = state > 0;
      }

      break;

    case 3:
      printf("Enter top lamp state, chain of 4x 0/1s: ");
      memset(buf, 0, sizeof(buf));
      n = scanf("%4s", buf);

      if (n > 0) {
        output->top_lamp_l1 = buf[0] == '1';
        output->top_lamp_l2 = buf[1] == '1';
        output->top_lamp_r1 = buf[2] == '1';
        output->top_lamp_r2 = buf[3] == '1';
      }

      break;

    case 4:
      output->light_bass = 1;

      output->top_lamp_l1 = 1;
      output->top_lamp_l2 = 1;
      output->top_lamp_r1 = 1;
      output->top_lamp_r2 = 1;

      output->pad_light_p1_up = 1;
      output->pad_light_p1_down = 1;
      output->pad_light_p1_left = 1;
      output->pad_light_p1_right = 1;

      output->pad_light_p2_up = 1;
      output->pad_light_p2_down = 1;
      output->pad_light_p2_left = 1;
      output->pad_light_p2_right = 1;

      break;

    case 5:
      output->light_bass = 0;

      output->top_lamp_l1 = 0;
      output->top_lamp_l2 = 0;
      output->top_lamp_r1 = 0;
      output->top_lamp_r2 = 0;

      output->pad_light_p1_up = 0;
      output->pad_light_p1_down = 0;
      output->pad_light_p1_left = 0;
      output->pad_light_p1_right = 0;

      output->pad_light_p2_up = 0;
      output->pad_light_p2_down = 0;
      output->pad_light_p2_left = 0;
      output->pad_light_p2_right = 0;

      break;
  }

  return true;
}

static void draw_itg_tui(
    struct piuio_itg_output_paket *output,
    const struct piuio_itg_input_paket *inputs,
    double io_time_sec)
{
  static uint8_t counter = 0;

  assert(output != NULL);
  assert(inputs != NULL);

  system("clear");
  printf("Press CTRL + C to open menu\n");
  printf("%d - I/O time: %.5f secs\n", counter++, io_time_sec);
  printf(
      "         %d+---------------+%d\n"
      "          |               |\n"
      "         %d+--+---------+--+%d\n"
      "             |         |\n"
      "           +-+---------+-+\n"
      "           |             |\n"
      "           |  %d       %d  |\n"
      "           |%d   %d   %d   %d|\n"
      "           |  %d       %d  |\n"
      "        +--+---+-----+---+--+\n"
      "        |      |SVC %d|      |\n"
      "        |      |TST %d|      |\n"
      "        |  %d   |CN  %d|  %d   |\n"
      "        |      |CC  %d|      |\n"
      "        |      |CLR %d|      |\n"
      "        +------+-----+------+\n"
      "+-----+-----+-----+-----+-----+-----+\n"
      "|     |  %d  |     |     |  %d  |     |\n"
      "|     |%d %d %d|     |     |%d %d %d|     |\n"
      "|     |  %d  |     |     |  %d  |     |\n"
      "+-----+-----+-----+-----+-----+-----+\n"
      "|  %d  |     |  %d  |  %d  |     |  %d  |\n"
      "|%d %d %d|     |%d %d %d|%d %d %d|     |%d %d %d|\n"
      "|  %d  |     |  %d  |  %d  |     |  %d  |\n"
      "+-----+-----+-----+-----+-----+-----+\n"
      "|     |  %d  |     |     |  %d  |     |\n"
      "|     |%d %d %d|     |     |%d %d %d|     |\n"
      "|     |  %d  |     |     |  %d  |     |\n"
      "+-----+-----+-----+-----+-----+-----+\n",
      output->top_lamp_l1,
      output->top_lamp_r1,
      output->top_lamp_l2,
      output->top_lamp_r2,
      // -----------------------------------------------------------------
      // Hacky, as this can swallow input states press -> release, release ->
      // press but good enough for testing. Same for other operator related
      // switches
      inputs[PIUIO_SENSOR_MASK_RIGHT].menu_p1_start |
          inputs[PIUIO_SENSOR_MASK_LEFT].menu_p1_start |
          inputs[PIUIO_SENSOR_MASK_DOWN].menu_p1_start |
          inputs[PIUIO_SENSOR_MASK_UP].menu_p1_start,
      inputs[PIUIO_SENSOR_MASK_RIGHT].menu_p2_start |
          inputs[PIUIO_SENSOR_MASK_LEFT].menu_p2_start |
          inputs[PIUIO_SENSOR_MASK_DOWN].menu_p2_start |
          inputs[PIUIO_SENSOR_MASK_UP].menu_p2_start,
      inputs[PIUIO_SENSOR_MASK_RIGHT].menu_p1_left |
          inputs[PIUIO_SENSOR_MASK_LEFT].menu_p1_left |
          inputs[PIUIO_SENSOR_MASK_DOWN].menu_p1_left |
          inputs[PIUIO_SENSOR_MASK_UP].menu_p1_left,
      inputs[PIUIO_SENSOR_MASK_RIGHT].menu_p1_right |
          inputs[PIUIO_SENSOR_MASK_LEFT].menu_p1_right |
          inputs[PIUIO_SENSOR_MASK_DOWN].menu_p1_right |
          inputs[PIUIO_SENSOR_MASK_UP].menu_p1_right,
      inputs[PIUIO_SENSOR_MASK_RIGHT].menu_p2_left |
          inputs[PIUIO_SENSOR_MASK_LEFT].menu_p2_left |
          inputs[PIUIO_SENSOR_MASK_DOWN].menu_p2_left |
          inputs[PIUIO_SENSOR_MASK_UP].menu_p2_left,
      inputs[PIUIO_SENSOR_MASK_RIGHT].menu_p2_right |
          inputs[PIUIO_SENSOR_MASK_LEFT].menu_p2_right |
          inputs[PIUIO_SENSOR_MASK_DOWN].menu_p2_right |
          inputs[PIUIO_SENSOR_MASK_UP].menu_p2_right,
      inputs[PIUIO_SENSOR_MASK_RIGHT].menu_p1_back |
          inputs[PIUIO_SENSOR_MASK_LEFT].menu_p1_back |
          inputs[PIUIO_SENSOR_MASK_DOWN].menu_p1_back |
          inputs[PIUIO_SENSOR_MASK_UP].menu_p1_back,
      inputs[PIUIO_SENSOR_MASK_RIGHT].menu_p2_back |
          inputs[PIUIO_SENSOR_MASK_LEFT].menu_p2_back |
          inputs[PIUIO_SENSOR_MASK_DOWN].menu_p2_back |
          inputs[PIUIO_SENSOR_MASK_UP].menu_p2_back,
      // -----------------------------------------------------------------
      inputs[PIUIO_SENSOR_MASK_RIGHT].service |
          inputs[PIUIO_SENSOR_MASK_LEFT].service |
          inputs[PIUIO_SENSOR_MASK_DOWN].service |
          inputs[PIUIO_SENSOR_MASK_UP].service,
      inputs[PIUIO_SENSOR_MASK_RIGHT].test |
          inputs[PIUIO_SENSOR_MASK_LEFT].test |
          inputs[PIUIO_SENSOR_MASK_DOWN].test |
          inputs[PIUIO_SENSOR_MASK_UP].test,
      output->light_bass,
      inputs[PIUIO_SENSOR_MASK_RIGHT].coin |
          inputs[PIUIO_SENSOR_MASK_LEFT].coin |
          inputs[PIUIO_SENSOR_MASK_DOWN].coin |
          inputs[PIUIO_SENSOR_MASK_UP].coin,
      output->light_bass,
      output->coin_counter,
      inputs[PIUIO_SENSOR_MASK_RIGHT].clear |
          inputs[PIUIO_SENSOR_MASK_LEFT].clear |
          inputs[PIUIO_SENSOR_MASK_DOWN].clear |
          inputs[PIUIO_SENSOR_MASK_UP].clear,
      // -----------------------------------------------------------------
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p1_up,
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p2_up,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p1_up,
      output->pad_light_p1_up,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p1_up,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p2_up,
      output->pad_light_p2_up,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p2_up,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p1_up,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p2_up,
      // -----------------------------------------------------------------
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p1_left,
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p1_right,
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p2_left,
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p2_right,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p1_left,
      output->pad_light_p1_left,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p1_left,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p1_right,
      output->pad_light_p1_right,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p1_right,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p2_left,
      output->pad_light_p2_left,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p2_left,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p2_right,
      output->pad_light_p2_right,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p2_right,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p1_left,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p1_right,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p2_left,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p2_right,
      // -----------------------------------------------------------------
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p1_down,
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p2_down,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p1_down,
      output->pad_light_p1_down,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p1_down,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p2_down,
      output->pad_light_p2_down,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p2_down,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p1_down,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p2_down);

  handle_pad_lights_on_input_itg(output, inputs);
}

static void draw_piu_tui(
    struct piuio_piu_output_paket *output,
    const struct piuio_piu_input_paket *inputs,
    double io_time_sec)
{
  static uint8_t counter = 0;

  assert(output != NULL);
  assert(inputs != NULL);

  system("clear");
  printf("Press CTRL + C to open menu\n");
  printf("%d - I/O time: %.5f secs\n", counter++, io_time_sec);
  printf(
      "             %d  %d  %d  %d\n"
      "          +---------------+\n"
      "          +--+---------+--+\n"
      "             |         |\n"
      "           +-+---------+-+\n"
      "           |             |\n"
      "           |             |\n"
      "           |             |\n"
      "           |             |\n"
      "        +--+---+-----+---+--+\n"
      "        |      |SVC %d|      |\n"
      "        |      |TST %d|      |\n"
      "        |  %d   |CN1 %d|  %d   |\n"
      "        |      |CN2 %d|      |\n"
      "        |CC1 %d |CLR %d|CC2 %d |\n"
      "        +------+-----+------+\n"
      "+-----+-----+-----+-----+-----+-----+\n"
      "|  %d  |     |  %d  |  %d  |     |  %d  |\n"
      "|%d %d %d|     |%d %d %d|%d %d %d|     |%d %d %d|\n"
      "|  %d  |     |  %d  |  %d  |     |  %d  |\n"
      "+-----+-----+-----+-----+-----+-----+\n"
      "|     |  %d  |     |     |  %d  |     |\n"
      "|     |%d %d %d|     |     |%d %d %d|     |\n"
      "|     |  %d  |     |     |  %d  |     |\n"
      "+-----+-----+-----+-----+-----+-----+\n"
      "|  %d  |     |  %d  |  %d  |     |  %d  |\n"
      "|%d %d %d|     |%d %d %d|%d %d %d|     |%d %d %d|\n"
      "|  %d  |     |  %d  |  %d  |     |  %d  |\n"
      "+-----+-----+-----+-----+-----+-----+\n",
      output->top_lamp_l1,
      output->top_lamp_l2,
      output->top_lamp_r1,
      output->top_lamp_r2,
      // -----------------------------------------------------------------
      // Hacky, as this can swallow input states press -> release, release ->
      // press but good enough for testing. Same for other operator related
      // switches
      inputs[PIUIO_SENSOR_MASK_RIGHT].service |
          inputs[PIUIO_SENSOR_MASK_LEFT].service |
          inputs[PIUIO_SENSOR_MASK_DOWN].service |
          inputs[PIUIO_SENSOR_MASK_UP].service,
      inputs[PIUIO_SENSOR_MASK_RIGHT].test |
          inputs[PIUIO_SENSOR_MASK_LEFT].test |
          inputs[PIUIO_SENSOR_MASK_DOWN].test |
          inputs[PIUIO_SENSOR_MASK_UP].test,
      output->light_bass,
      inputs[PIUIO_SENSOR_MASK_RIGHT].coin_1 |
          inputs[PIUIO_SENSOR_MASK_LEFT].coin_1 |
          inputs[PIUIO_SENSOR_MASK_DOWN].coin_1 |
          inputs[PIUIO_SENSOR_MASK_UP].coin_1,
      output->light_bass,
      inputs[PIUIO_SENSOR_MASK_RIGHT].coin_2 |
          inputs[PIUIO_SENSOR_MASK_LEFT].coin_2 |
          inputs[PIUIO_SENSOR_MASK_DOWN].coin_2 |
          inputs[PIUIO_SENSOR_MASK_UP].coin_2,
      output->coin_counter_1,
      inputs[PIUIO_SENSOR_MASK_RIGHT].clear |
          inputs[PIUIO_SENSOR_MASK_LEFT].clear |
          inputs[PIUIO_SENSOR_MASK_DOWN].clear |
          inputs[PIUIO_SENSOR_MASK_UP].clear,
      output->coin_counter_2,
      // -----------------------------------------------------------------
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p1_lu,
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p1_ru,
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p2_lu,
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p2_ru,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p1_lu,
      output->pad_light_p1_lu,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p1_lu,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p1_ru,
      output->pad_light_p1_ru,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p1_ru,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p2_lu,
      output->pad_light_p2_lu,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p2_lu,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p2_ru,
      output->pad_light_p2_ru,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p2_ru,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p1_lu,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p1_ru,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p2_lu,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p2_ru,
      // -----------------------------------------------------------------
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p1_cn,
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p2_cn,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p1_cn,
      output->pad_light_p1_cn,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p1_cn,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p2_cn,
      output->pad_light_p2_cn,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p2_cn,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p1_cn,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p2_cn,
      // -----------------------------------------------------------------
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p1_ld,
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p1_rd,
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p2_ld,
      inputs[PIUIO_SENSOR_MASK_UP].sensor_p2_rd,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p1_ld,
      output->pad_light_p1_ld,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p1_ld,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p1_rd,
      output->pad_light_p1_rd,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p1_rd,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p2_ld,
      output->pad_light_p2_ld,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p2_ld,
      inputs[PIUIO_SENSOR_MASK_LEFT].sensor_p2_rd,
      output->pad_light_p2_rd,
      inputs[PIUIO_SENSOR_MASK_RIGHT].sensor_p2_rd,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p1_ld,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p1_rd,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p2_ld,
      inputs[PIUIO_SENSOR_MASK_DOWN].sensor_p2_rd);

  handle_pad_lights_on_input_piu(output, inputs);
}

// -----------------------------------------------------------------------------------------

static bool render_raw_piu(
    union piuio_output_paket *output,
    const struct piuio_usb_input_batch_paket *input,
    double io_time_sec)
{
  assert(output != NULL);
  assert(input != NULL);

  for (size_t i = 0; i < sizeof(output->raw); i++) {
    printf("%02X ", output->raw[i]);
  }

  for (size_t i = 0; i < PIUIO_SENSOR_MASK_TOTAL_COUNT; i++) {
    for (size_t j = 0; j < sizeof(input->pakets[i].raw); j++) {
      printf("%02X ", input->pakets[i].raw[j]);
    }
  }

  printf("\n");

  handle_pad_lights_on_input_piu(&output->piu, &input->pakets->piu);

  return !interrupted;
}

static bool render_raw_itg(
    union piuio_output_paket *output,
    const struct piuio_usb_input_batch_paket *input,
    double io_time_sec)
{
  assert(output != NULL);
  assert(input != NULL);

  for (size_t i = 0; i < sizeof(output->raw); i++) {
    printf("%02X ", output->raw[i]);
  }

  for (size_t i = 0; i < PIUIO_SENSOR_MASK_TOTAL_COUNT; i++) {
    for (size_t j = 0; j < sizeof(input->pakets[i].raw); j++) {
      printf("%02X ", input->pakets[i].raw[j]);
    }
  }

  printf("\n");

  handle_pad_lights_on_input_itg(&output->itg, &input->pakets->itg);

  return !interrupted;
}

static bool render_text_piu(
    union piuio_output_paket *output,
    const struct piuio_usb_input_batch_paket *input,
    double io_time_sec)
{
  assert(output != NULL);
  assert(input != NULL);

  static uint8_t counter = 0;

  system("clear");
  printf(
      "%d - I/O time: %.5f secs\n"
      "p1_lu: %d|%d|%d|%d\n"
      "p1_ru: %d|%d|%d|%d\n"
      "p1_cn: %d|%d|%d|%d\n"
      "p1_ld: %d|%d|%d|%d\n"
      "p1_rd: %d|%d|%d|%d\n"
      "p2_lu: %d|%d|%d|%d\n"
      "p2_ru: %d|%d|%d|%d\n"
      "p2_cn: %d|%d|%d|%d\n"
      "p2_ld: %d|%d|%d|%d\n"
      "p2_rd: %d|%d|%d|%d\n"
      "Test %d, Service %d, Coin1 %d, Coin2 %d, Clear %d\n",
      counter++,
      io_time_sec,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.sensor_p1_lu,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.sensor_p1_lu,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.sensor_p1_lu,
      input->pakets[PIUIO_SENSOR_MASK_UP].piu.sensor_p1_lu,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.sensor_p1_ru,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.sensor_p1_ru,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.sensor_p1_ru,
      input->pakets[PIUIO_SENSOR_MASK_UP].piu.sensor_p1_ru,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.sensor_p1_cn,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.sensor_p1_cn,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.sensor_p1_cn,
      input->pakets[PIUIO_SENSOR_MASK_UP].piu.sensor_p1_cn,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.sensor_p1_ld,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.sensor_p1_ld,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.sensor_p1_ld,
      input->pakets[PIUIO_SENSOR_MASK_UP].piu.sensor_p1_ld,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.sensor_p1_rd,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.sensor_p1_rd,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.sensor_p1_rd,
      input->pakets[PIUIO_SENSOR_MASK_UP].piu.sensor_p1_rd,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.sensor_p2_lu,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.sensor_p2_lu,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.sensor_p2_lu,
      input->pakets[PIUIO_SENSOR_MASK_UP].piu.sensor_p2_lu,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.sensor_p2_ru,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.sensor_p2_ru,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.sensor_p2_ru,
      input->pakets[PIUIO_SENSOR_MASK_UP].piu.sensor_p2_ru,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.sensor_p2_cn,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.sensor_p2_cn,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.sensor_p2_cn,
      input->pakets[PIUIO_SENSOR_MASK_UP].piu.sensor_p2_cn,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.sensor_p2_ld,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.sensor_p2_ld,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.sensor_p2_ld,
      input->pakets[PIUIO_SENSOR_MASK_UP].piu.sensor_p2_ld,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.sensor_p2_rd,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.sensor_p2_rd,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.sensor_p2_rd,
      input->pakets[PIUIO_SENSOR_MASK_UP].piu.sensor_p2_rd,
      // -----
      // Don't care about debouncing any of these inputs, just merge the polls
      // which is good enough for testing as we do not have to be careful with
      // stuff like losing coin drops
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.test |
          input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.test |
          input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.test |
          input->pakets[PIUIO_SENSOR_MASK_UP].piu.test,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.service |
          input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.service |
          input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.service |
          input->pakets[PIUIO_SENSOR_MASK_UP].piu.service,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.coin_1 |
          input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.coin_1 |
          input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.coin_1 |
          input->pakets[PIUIO_SENSOR_MASK_UP].piu.coin_1,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.coin_2 |
          input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.coin_2 |
          input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.coin_2 |
          input->pakets[PIUIO_SENSOR_MASK_UP].piu.coin_2,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].piu.clear |
          input->pakets[PIUIO_SENSOR_MASK_LEFT].piu.clear |
          input->pakets[PIUIO_SENSOR_MASK_DOWN].piu.clear |
          input->pakets[PIUIO_SENSOR_MASK_UP].piu.clear);

  handle_pad_lights_on_input_piu(&output->piu, &input->pakets->piu);

  return !interrupted;
}

static bool render_text_itg(
    union piuio_output_paket *output,
    const struct piuio_usb_input_batch_paket *input,
    double io_time_sec)
{
  assert(output != NULL);
  assert(input != NULL);

  static uint8_t counter = 0;

  system("clear");
  printf(
      "%d - I/O time: %.5f secs\n"
      "p1_u: %d|%d|%d|%d\n"
      "p1_d: %d|%d|%d|%d\n"
      "p1_l: %d|%d|%d|%d\n"
      "p1_r: %d|%d|%d|%d\n"
      "p2_u: %d|%d|%d|%d\n"
      "p2_d: %d|%d|%d|%d\n"
      "p2_l: %d|%d|%d|%d\n"
      "p2_r: %d|%d|%d|%d\n"
      "Test %d, Service %d, Coin %d, Clear %d\n",
      counter++,
      io_time_sec,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].itg.sensor_p1_up,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].itg.sensor_p1_up,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].itg.sensor_p1_up,
      input->pakets[PIUIO_SENSOR_MASK_UP].itg.sensor_p1_up,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].itg.sensor_p1_down,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].itg.sensor_p1_down,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].itg.sensor_p1_down,
      input->pakets[PIUIO_SENSOR_MASK_UP].itg.sensor_p1_down,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].itg.sensor_p1_left,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].itg.sensor_p1_left,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].itg.sensor_p1_left,
      input->pakets[PIUIO_SENSOR_MASK_UP].itg.sensor_p1_left,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].itg.sensor_p1_right,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].itg.sensor_p1_right,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].itg.sensor_p1_right,
      input->pakets[PIUIO_SENSOR_MASK_UP].itg.sensor_p1_right,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].itg.sensor_p2_up,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].itg.sensor_p2_up,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].itg.sensor_p2_up,
      input->pakets[PIUIO_SENSOR_MASK_UP].itg.sensor_p2_up,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].itg.sensor_p2_down,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].itg.sensor_p2_down,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].itg.sensor_p2_down,
      input->pakets[PIUIO_SENSOR_MASK_UP].itg.sensor_p2_down,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].itg.sensor_p2_left,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].itg.sensor_p2_left,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].itg.sensor_p2_left,
      input->pakets[PIUIO_SENSOR_MASK_UP].itg.sensor_p2_left,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].itg.sensor_p2_right,
      input->pakets[PIUIO_SENSOR_MASK_LEFT].itg.sensor_p2_right,
      input->pakets[PIUIO_SENSOR_MASK_DOWN].itg.sensor_p2_right,
      input->pakets[PIUIO_SENSOR_MASK_UP].itg.sensor_p2_right,
      // -----
      // Don't care about debouncing any of these inputs, just merge the polls
      // which is good enough for testing as we do not have to be careful with
      // stuff like losing coin drops
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].itg.test |
          input->pakets[PIUIO_SENSOR_MASK_LEFT].itg.test |
          input->pakets[PIUIO_SENSOR_MASK_DOWN].itg.test |
          input->pakets[PIUIO_SENSOR_MASK_UP].itg.test,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].itg.service |
          input->pakets[PIUIO_SENSOR_MASK_LEFT].itg.service |
          input->pakets[PIUIO_SENSOR_MASK_DOWN].itg.service |
          input->pakets[PIUIO_SENSOR_MASK_UP].itg.service,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].itg.coin |
          input->pakets[PIUIO_SENSOR_MASK_LEFT].itg.coin |
          input->pakets[PIUIO_SENSOR_MASK_DOWN].itg.coin |
          input->pakets[PIUIO_SENSOR_MASK_UP].itg.coin,
      // -----
      input->pakets[PIUIO_SENSOR_MASK_RIGHT].itg.clear |
          input->pakets[PIUIO_SENSOR_MASK_LEFT].itg.clear |
          input->pakets[PIUIO_SENSOR_MASK_DOWN].itg.clear |
          input->pakets[PIUIO_SENSOR_MASK_UP].itg.clear);

  handle_pad_lights_on_input_itg(&output->itg, &input->pakets->itg);

  return !interrupted;
}

static bool render_tui_piu(
    union piuio_output_paket *output,
    const struct piuio_usb_input_batch_paket *input,
    double io_time_sec)
{
  assert(output != NULL);
  assert(input != NULL);

  handle_pad_lights_on_input_piu(&output->piu, &input->pakets->piu);

  if (interrupted) {
    if (draw_menu_piu_tui(&output->piu)) {
      interrupted = false;
    } else {
      return false;
    }
  }

  draw_piu_tui(
      &output->piu,
      (struct piuio_piu_input_paket *) input->pakets,
      io_time_sec);

  return true;
}

static bool render_tui_itg(
    union piuio_output_paket *output,
    const struct piuio_usb_input_batch_paket *input,
    double io_time_sec)
{
  assert(output != NULL);
  assert(input != NULL);

  handle_pad_lights_on_input_itg(&output->itg, &input->pakets->itg);

  if (interrupted) {
    if (draw_menu_itg_tui(&output->itg)) {
      interrupted = false;
    } else {
      return false;
    }
  }

  draw_itg_tui(
      &output->itg,
      (struct piuio_itg_input_paket *) input->pakets,
      io_time_sec);

  return true;
}

static bool render_benchmark(
    union piuio_output_paket *output,
    const struct piuio_usb_input_batch_paket *input,
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
  union piuio_output_paket output;
  struct piuio_usb_input_batch_paket input;
  struct timespec tstart;
  struct timespec tend;
  double io_time_sec;
  bool loop;

  assert(render);

  memset(&tstart, 0, sizeof(struct timespec));
  memset(&tend, 0, sizeof(struct timespec));

  memset(output.raw, 0, sizeof(output.raw));
  memset(&input, 0, sizeof(struct piuio_usb_input_batch_paket));

  result = piuio_usb_open(&handle);

  if (result) {
    errno = result;
    perror("Opening PIUIO failed");
    exit(EXIT_FAILURE);
  }

  loop = true;

  while (loop) {
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    result = piuio_usb_poll_full_cycle(handle, &output, &input);

    clock_gettime(CLOCK_MONOTONIC, &tend);

    io_time_sec = ((double) tend.tv_sec + 1.0e-9 * tend.tv_nsec) -
        ((double) tstart.tv_sec + 1.0e-9 * tstart.tv_nsec);

    if (result) {
      errno = result;
      perror("Running update cycle for PIUIO failed");
      exit(EXIT_FAILURE);
    }

    loop = render(&output, &input, io_time_sec);

    sleep_ms(delay_ms);
  }

  piuio_usb_close(handle);
}

static void proc_kmod(int32_t delay_ms, func_render_data_t render)
{
  int32_t fd;
  int32_t result;
  union piuio_kmod_paket paket;
  struct timespec tstart;
  struct timespec tend;
  double io_time_sec;
  bool loop;

  assert(render);

  memset(&tstart, 0, sizeof(struct timespec));
  memset(&tend, 0, sizeof(struct timespec));

  // Null largest field spanning entire union
  memset(&paket, 0, sizeof(paket));

  result = piuio_kmod_open(&fd);

  if (result) {
    errno = result;
    perror("Opening PIUIO kmod failed");
    exit(EXIT_FAILURE);
  }

  loop = true;

  while (loop) {
    // Output is overwriten by kernel module
    // Comply with output of other mode and save 0'd output data
    // To print correct values
    uint8_t output_buffer[sizeof(paket)];

    memset(output_buffer, 0, sizeof(output_buffer));
    memcpy(&paket, output_buffer, sizeof(output_buffer));

    clock_gettime(CLOCK_MONOTONIC, &tstart);

    result = piuio_kmod_poll(fd, &paket);

    clock_gettime(CLOCK_MONOTONIC, &tend);

    io_time_sec = ((double) tend.tv_sec + 1.0e-9 * tend.tv_nsec) -
        ((double) tstart.tv_sec + 1.0e-9 * tstart.tv_nsec);

    if (result) {
      errno = result;
      perror("Running update cycle for PIUIO kmod failed");
      exit(EXIT_FAILURE);
    }

    loop = render(
        (union piuio_output_paket *) output_buffer, &paket.input, io_time_sec);

    sleep_ms(delay_ms);
  }

  piuio_kmod_close(fd);
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

  if (options.mode == MODE_RAW && options.type == TYPE_USB &&
      options.game == GAME_PIU) {
    proc_usb(options.delay_ms, render_raw_piu);
  } else if (
      options.mode == MODE_RAW && options.type == TYPE_USB &&
      options.game == GAME_ITG) {
    proc_usb(options.delay_ms, render_raw_itg);
  } else if (
      options.mode == MODE_RAW && options.type == TYPE_KMOD &&
      options.game == GAME_PIU) {
    proc_kmod(options.delay_ms, render_raw_piu);
  } else if (
      options.mode == MODE_RAW && options.type == TYPE_KMOD &&
      options.game == GAME_ITG) {
    proc_kmod(options.delay_ms, render_raw_itg);
  } else if (
      options.mode == MODE_TEXT && options.type == TYPE_USB &&
      options.game == GAME_PIU) {
    proc_usb(options.delay_ms, render_text_piu);
  } else if (
      options.mode == MODE_TEXT && options.type == TYPE_USB &&
      options.game == GAME_ITG) {
    proc_usb(options.delay_ms, render_text_itg);
  } else if (
      options.mode == MODE_TEXT && options.type == TYPE_KMOD &&
      options.game == GAME_PIU) {
    proc_kmod(options.delay_ms, render_text_piu);
  } else if (
      options.mode == MODE_TEXT && options.type == TYPE_KMOD &&
      options.game == GAME_ITG) {
    proc_kmod(options.delay_ms, render_text_itg);
  } else if (
      options.mode == MODE_TUI && options.type == TYPE_USB &&
      options.game == GAME_PIU) {
    proc_usb(options.delay_ms, render_tui_piu);
  } else if (
      options.mode == MODE_TUI && options.type == TYPE_USB &&
      options.game == GAME_ITG) {
    proc_usb(options.delay_ms, render_tui_itg);
  } else if (
      options.mode == MODE_TUI && options.type == TYPE_KMOD &&
      options.game == GAME_PIU) {
    proc_kmod(options.delay_ms, render_tui_piu);
  } else if (
      options.mode == MODE_TUI && options.type == TYPE_KMOD &&
      options.game == GAME_ITG) {
    proc_kmod(options.delay_ms, render_tui_itg);
  } else if (options.mode == MODE_BENCHMARK && options.type == TYPE_USB) {
    proc_usb(options.delay_ms, render_benchmark);
  } else if (options.mode == MODE_BENCHMARK && options.type == TYPE_KMOD) {
    proc_kmod(options.delay_ms, render_benchmark);
  } else {
    fprintf(stderr, "Invalid parameters selected\n");
    print_usage(argv);
  }

  return EXIT_SUCCESS;
}
