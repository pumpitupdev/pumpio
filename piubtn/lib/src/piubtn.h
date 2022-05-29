/**
 * Data structures for mapping input and output data of the PIUBTN usb device
 * for Pump It Up (Pro).
 */
#ifndef PIUBTN_H
#define PIUBTN_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define PIUBTN_INPUT_PAKET_SIZE 8
#define PIUBTN_OUTPUT_PAKET_SIZE 8

/**
 * Single output paket
 */
struct __attribute__((__packed__)) piubtn_piu_output_paket {
  /* Byte 0 */
  uint8_t btn_light_p2_start : 1;
  uint8_t btn_light_p2_right : 1;
  uint8_t btn_light_p2_left : 1;
  uint8_t btn_light_p2_back : 1;
  uint8_t btn_light_p1_start : 1;
  uint8_t btn_light_p1_right : 1;
  uint8_t btn_light_p1_left : 1;
  uint8_t btn_light_p1_back : 1;

  /* Byte 1 */
  uint8_t unused_1;

  /* Byte 2 */
  uint8_t unused_2;

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
 * "Polymorphic abstraction" of a single output paket
 */
union piubtn_output_paket {
  struct piubtn_piu_output_paket piu;
  uint8_t raw[PIUBTN_OUTPUT_PAKET_SIZE];
};

/**
 * Single input paket
 */
struct __attribute__((__packed__)) piubtn_piu_input_paket {
  /* Byte 0 */
  uint8_t btn_p1_back : 1;
  uint8_t btn_p1_left : 1;
  uint8_t btn_p1_right : 1;
  uint8_t btn_p1_start : 1;
  uint8_t btn_p2_back : 1;
  uint8_t btn_p2_left : 1;
  uint8_t btn_p2_right : 1;
  uint8_t btn_p2_start : 1;

  /* Byte 1 */
  uint8_t unused_1;

  /* Byte 2 */
  uint8_t unused_2;

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
union piubtn_input_paket {
  struct piubtn_piu_input_paket piu;
  uint8_t raw[PIUBTN_INPUT_PAKET_SIZE];
};

static_assert(
    sizeof(struct piubtn_piu_output_paket) == PIUBTN_OUTPUT_PAKET_SIZE,
    "Expected size of piubtn_output_paket incorrect");
static_assert(
    sizeof(struct piubtn_piu_input_paket) == PIUBTN_INPUT_PAKET_SIZE,
    "Expected size of piubtn_piu_input_paket incorrect");
static_assert(
    sizeof(union piubtn_output_paket) == PIUBTN_OUTPUT_PAKET_SIZE,
    "Expected size of piubtn_output_paket incorrect");
static_assert(
    sizeof(union piubtn_input_paket) == PIUBTN_INPUT_PAKET_SIZE,
    "Expected size of piubtn_input_paket incorrect");

#endif