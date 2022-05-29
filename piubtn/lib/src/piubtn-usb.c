#include <assert.h>
#include <errno.h>

#include "piubtn-usb.h"
#include "usb_.h"

#define PIUBTN_USB_VID 0x0D2F
#define PIUBTN_USB_PID 0x1010

#define PIUBTN_USB_CONFIG 0x01
#define PIUBTN_USB_IFACE 0x00

#define PIUBTN_USB_CTRL_TYPE_IN 0xC0
#define PIUBTN_USB_CTRL_TYPE_OUT 0x40
#define PIUBTN_USB_CTRL_REQUEST 0xAE
#define PIUBTN_USB_REQ_TIMEOUT 10000

bool piubtn_usb_available()
{
  return pumpio_usb_available(PIUBTN_USB_VID, PIUBTN_USB_PID);
}

result_t piubtn_usb_open(void **handle)
{
  assert(handle != NULL);

  return pumpio_usb_open(
      handle, PIUBTN_USB_VID, PIUBTN_USB_PID, PIUBTN_USB_CONFIG, PIUBTN_USB_IFACE);
}

result_t piubtn_usb_poll(
    void *handle,
    const union piubtn_output_paket *output,
    union piubtn_input_paket *input)
{
  result_t result;
  uint16_t res_len;

  assert(handle != NULL);
  assert(output != NULL);
  assert(input != NULL);

  // Write outputs
  result = pumpio_usb_control_transfer(
      handle,
      PIUBTN_USB_CTRL_TYPE_OUT,
      PIUBTN_USB_CTRL_REQUEST,
      0,
      0,
      (uint8_t *) output->raw,
      sizeof(output->raw),
      PIUBTN_USB_REQ_TIMEOUT,
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
      PIUBTN_USB_CTRL_TYPE_IN,
      PIUBTN_USB_CTRL_REQUEST,
      0,
      0,
      (uint8_t *) input->raw,
      sizeof(input->raw),
      PIUBTN_USB_REQ_TIMEOUT,
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

void piubtn_usb_close(void *handle)
{
  assert(handle != NULL);

  pumpio_usb_close(handle);
}