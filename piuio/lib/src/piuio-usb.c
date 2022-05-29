#include <assert.h>
#include <errno.h>

#include "piuio-usb.h"
#include "usb_.h"

#define PIUIO_USB_VID 0x0547
#define PIUIO_USB_PID 0x1002

#define PIUIO_USB_CONFIG 0x01
#define PIUIO_USB_IFACE 0x00

#define PIUIO_USB_CTRL_TYPE_IN 0xC0
#define PIUIO_USB_CTRL_TYPE_OUT 0x40
#define PIUIO_USB_CTRL_REQUEST 0xAE
#define PIUIO_USB_REQ_TIMEOUT 10000

bool piuio_usb_available()
{
  return pumpio_usb_available(PIUIO_USB_VID, PIUIO_USB_PID);
}

result_t piuio_usb_open(void **handle)
{
  assert(handle != NULL);

  return pumpio_usb_open(
      handle, PIUIO_USB_VID, PIUIO_USB_PID, PIUIO_USB_CONFIG, PIUIO_USB_IFACE);
}

result_t piuio_usb_poll_one_cycle(
    void *handle,
    const union piuio_output_paket *output,
    union piuio_input_paket *input)
{
  result_t result;
  uint16_t res_len;

  assert(handle != NULL);
  assert(output != NULL);
  assert(input != NULL);

  // Write outputs
  result = pumpio_usb_control_transfer(
      handle,
      PIUIO_USB_CTRL_TYPE_OUT,
      PIUIO_USB_CTRL_REQUEST,
      0,
      0,
      (uint8_t *) output->raw,
      sizeof(output->raw),
      PIUIO_USB_REQ_TIMEOUT,
      &res_len);

  if (RESULT_IS_ERROR(result)) {
    return result;
  }

  if (res_len != sizeof(output->raw)) {
    return EIO;
  }

  // Read inputs
  result = pumpio_usb_control_transfer(
      handle,
      PIUIO_USB_CTRL_TYPE_IN,
      PIUIO_USB_CTRL_REQUEST,
      0,
      0,
      (uint8_t *) input->raw,
      sizeof(input->raw),
      PIUIO_USB_REQ_TIMEOUT,
      &res_len);

  if (RESULT_IS_ERROR(result)) {
    return result;
  }

  if (res_len != sizeof(input->raw)) {
    return EIO;
  }

  // Invert pull ups
  for (uint8_t j = 0; j < sizeof(input->raw); j++) {
    input->raw[j] ^= 0xFF;
  }

  return RESULT_SUCCESS;
}

result_t piuio_usb_poll_full_cycle(
    void *handle,
    union piuio_output_paket *output,
    struct piuio_usb_input_batch_paket *input)
{
  result_t result;
  uint16_t res_len;

  assert(handle != NULL);
  assert(output != NULL);
  assert(input != NULL);

  for (uint8_t i = 0; i < PIUIO_SENSOR_MASK_TOTAL_COUNT; i++) {
    // Cycle sensor mask, itg and piu have sensor mask on same bits
    output->piu.sensor_mask = i;

    // Write outputs
    result = pumpio_usb_control_transfer(
        handle,
        PIUIO_USB_CTRL_TYPE_OUT,
        PIUIO_USB_CTRL_REQUEST,
        0,
        0,
        (uint8_t *) output->raw,
        sizeof(output->raw),
        PIUIO_USB_REQ_TIMEOUT,
        &res_len);

    if (RESULT_IS_ERROR(result)) {
      return result;
    }

    if (res_len != sizeof(output->raw)) {
      return EIO;
    }

    // Read inputs, doesn't matter which struct we use here, using piu as
    // default
    result = pumpio_usb_control_transfer(
        handle,
        PIUIO_USB_CTRL_TYPE_IN,
        PIUIO_USB_CTRL_REQUEST,
        0,
        0,
        (uint8_t *) &input->pakets[i].raw,
        sizeof(input->pakets[i].raw),
        PIUIO_USB_REQ_TIMEOUT,
        &res_len);

    if (RESULT_IS_ERROR(result)) {
      return result;
    }

    if (res_len != sizeof(input->pakets[i].raw)) {
      return EIO;
    }

    // Invert pull ups
    for (uint8_t j = 0; j < sizeof(input->pakets[i].raw); j++) {
      input->pakets[i].raw[j] ^= 0xFF;
    }
  }

  return RESULT_SUCCESS;
}

void piuio_usb_close(void *handle)
{
  assert(handle != NULL);

  pumpio_usb_close(handle);
}