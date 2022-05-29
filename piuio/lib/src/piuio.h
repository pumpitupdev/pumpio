/**
 * Data structures for mapping input and output data of the PIUIO usb device
 * for Pump It Up and In The Groove. Both games use the same I/O hardware with
 * slightly different mappings.
 */
#ifndef PIUIO_H
#define PIUIO_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define PIUIO_INPUT_PAKET_SIZE 8
#define PIUIO_OUTPUT_PAKET_SIZE 8

/**
 * Sensor mask bits to select the sensor to be queried on the next input poll.
 *
 * Bits: 00 right, 01 left, 10 down, 11 up
 */
enum piuio_sensor_mask {
  PIUIO_SENSOR_MASK_RIGHT = 0,
  PIUIO_SENSOR_MASK_LEFT = 1,
  PIUIO_SENSOR_MASK_DOWN = 2,
  PIUIO_SENSOR_MASK_UP = 3,
  PIUIO_SENSOR_MASK_TOTAL_COUNT = 4,
};

/**
 * Single output paket struct for Pump It Up
 */
struct __attribute__((__packed__)) piuio_piu_output_paket {
  /* Byte 0 */
  uint8_t sensor_mask : 2;
  uint8_t pad_light_p1_lu : 1;
  uint8_t pad_light_p1_ru : 1;
  uint8_t pad_light_p1_cn : 1;
  uint8_t pad_light_p1_ld : 1;
  uint8_t pad_light_p1_rd : 1;
  uint8_t unused_0 : 1;

  /* Byte 1 */
  uint8_t unused_1 : 2;
  uint8_t light_bass : 1;
  uint8_t coin_counter_2 : 1;
  uint8_t unused_2 : 4;

  /* Byte 2 */
  uint8_t unsued_3 : 2;
  uint8_t pad_light_p2_lu : 1;
  uint8_t pad_light_p2_ru : 1;
  uint8_t pad_light_p2_cn : 1;
  uint8_t pad_light_p2_ld : 1;
  uint8_t pad_light_p2_rd : 1;
  uint8_t top_lamp_r2 : 1;

  /* Byte 3 */
  uint8_t top_lamp_r1 : 1;
  uint8_t top_lamp_l2 : 1;
  uint8_t top_lamp_l1 : 1;
  uint8_t unused_4 : 1;
  uint8_t coin_counter_1 : 1;
  uint8_t unused_5 : 3;

  /* Byte 4 */
  uint8_t unused_6;

  /* Byte 5 */
  uint8_t unused_7;

  /* Byte 6 */
  uint8_t unused_8;

  /* Byte 7 */
  uint8_t unsued_9;
};

/**
 * Single output paket struct for In The Groove
 */
struct __attribute__((__packed__)) piuio_itg_output_paket {
  /* Byte 0 */
  uint8_t sensor_mask : 2;
  uint8_t pad_light_p1_up : 1;
  uint8_t pad_light_p1_down : 1;
  uint8_t pad_light_p1_left : 1;
  uint8_t pad_light_p1_right : 1;
  uint8_t unused_0 : 2;

  /* Byte 1 */
  uint8_t unused_1 : 2;
  uint8_t light_bass : 1;
  uint8_t unused_2 : 5;

  /* Byte 2 */
  uint8_t unsued_3 : 2;
  uint8_t pad_light_p2_up : 1;
  uint8_t pad_light_p2_down : 1;
  uint8_t pad_light_p2_left : 1;
  uint8_t pad_light_p2_right : 1;
  uint8_t unused_4 : 1;
  uint8_t top_lamp_r2 : 1;

  /* Byte 3 */
  uint8_t top_lamp_r1 : 1;
  uint8_t top_lamp_l2 : 1;
  uint8_t top_lamp_l1 : 1;
  uint8_t unused_5 : 1;
  uint8_t coin_counter : 1;
  uint8_t unused_6 : 3;

  /* Byte 4 */
  uint8_t unused_7;

  /* Byte 5 */
  uint8_t unused_8;

  /* Byte 6 */
  uint8_t unused_9;

  /* Byte 7 */
  uint8_t unsued_10;
};

/**
 * "Polymorphic abstraction" of a single output paket
 */
union piuio_output_paket {
  struct piuio_piu_output_paket piu;
  struct piuio_itg_output_paket itg;
  uint8_t raw[PIUIO_OUTPUT_PAKET_SIZE];
};

/**
 * Single input paket struct for Pump It Up
 */
struct __attribute__((__packed__)) piuio_piu_input_paket {
  /* Byte 0 */
  uint8_t sensor_p1_lu : 1;
  uint8_t sensor_p1_ru : 1;
  uint8_t sensor_p1_cn : 1;
  uint8_t sensor_p1_ld : 1;
  uint8_t sensor_p1_rd : 1;
  uint8_t unused_0 : 3;

  /* Byte 1 */
  uint8_t unused_1 : 1;
  uint8_t test : 1;
  uint8_t coin_1 : 1;
  uint8_t unused_2 : 3;
  uint8_t service : 1;
  uint8_t clear : 1;

  /* Byte 2 */
  uint8_t sensor_p2_lu : 1;
  uint8_t sensor_p2_ru : 1;
  uint8_t sensor_p2_cn : 1;
  uint8_t sensor_p2_ld : 1;
  uint8_t sensor_p2_rd : 1;
  uint8_t unused_3 : 3;

  /* Byte 3 */
  uint8_t unused_4 : 2;
  uint8_t coin_2 : 1;
  uint8_t unused_5 : 5;

  /* Byte 4 */
  uint8_t unused_6;

  /* Byte 5 */
  uint8_t unused_7;

  /* Byte 6 */
  uint8_t unused_8;

  /* Byte 7 */
  uint8_t unsued_9;
};

/**
 * Single output paket struct for In The Groove
 */
struct __attribute__((__packed__)) piuio_itg_input_paket {
  /* Byte 0 */
  uint8_t sensor_p1_up : 1;
  uint8_t sensor_p1_down : 1;
  uint8_t sensor_p1_left : 1;
  uint8_t sensor_p1_right : 1;
  uint8_t menu_p1_start : 1;
  uint8_t menu_p1_back : 1;
  uint8_t menu_p1_left : 1;
  uint8_t menu_p1_right : 1;

  /* Byte 1 */
  uint8_t unused_1 : 1;
  uint8_t test : 1;
  uint8_t coin : 1;
  uint8_t unused_2 : 3;
  uint8_t service : 1;
  uint8_t clear : 1;

  /* Byte 2 */
  uint8_t sensor_p2_up : 1;
  uint8_t sensor_p2_down : 1;
  uint8_t sensor_p2_left : 1;
  uint8_t sensor_p2_right : 1;
  uint8_t menu_p2_start : 1;
  uint8_t menu_p2_back : 1;
  uint8_t menu_p2_left : 1;
  uint8_t menu_p2_right : 1;

  /* Byte 3 */
  uint8_t unused_3;

  /* Byte 4 */
  uint8_t unused_4;

  /* Byte 5 */
  uint8_t unused_5;

  /* Byte 6 */
  uint8_t unused_6;

  /* Byte 7 */
  uint8_t unsued_7;
};

/**
 * "Polymorphic abstraction" of a single input paket
 */
union piuio_input_paket {
  struct piuio_piu_input_paket piu;
  struct piuio_itg_input_paket itg;
  uint8_t raw[PIUIO_INPUT_PAKET_SIZE];
};

/**
 * A batch of four input pakets.
 *
 * As each panel of the stage has four sensors and one input paket can only
 * contain data for a single sensor, we need to issue four queries to the I/O
 * to get data for all sensors creating one complete input update cycle.
 */
struct __attribute__((__packed__)) piuio_usb_input_batch_paket {
  union piuio_input_paket pakets[PIUIO_SENSOR_MASK_TOTAL_COUNT];
};

static_assert(
    sizeof(struct piuio_piu_output_paket) == PIUIO_OUTPUT_PAKET_SIZE,
    "Expected size of piuio_piu_output_paket incorrect");
static_assert(
    sizeof(struct piuio_itg_output_paket) == PIUIO_OUTPUT_PAKET_SIZE,
    "Expected size of piuio_itg_output_paket incorrect");
static_assert(
    sizeof(struct piuio_piu_input_paket) == PIUIO_INPUT_PAKET_SIZE,
    "Expected size of piuio_piu_input_paket incorrect");
static_assert(
    sizeof(struct piuio_itg_input_paket) == PIUIO_INPUT_PAKET_SIZE,
    "Expected size of piuio_itg_input_paket incorrect");

#endif