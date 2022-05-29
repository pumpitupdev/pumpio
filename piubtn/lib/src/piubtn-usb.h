/**
 * Device level abstraction for opening, polling and closing a PIUBTN usb device
 * backed by the user-space USB library libusb.
 */
#ifndef PIUBTN_USB_H_
#define PIUBTN_USB_H_

#include <stdbool.h>
#include <stdint.h>

#include "piubtn.h"
#include "result.h"

/**
 * Check if a PIUBTN device is connected via USB and available to be opened.
 *
 * @return True if device is connected, false otherwise.
 */
bool piubtn_usb_available();

/**
 * Open a connected PIUBTN device using a user space usb library.
 * 
 * @param handle Pointer to variable (void*) to store the resulting handle
 *               reference in if the call is successful. The caller is
 *               responsible for managing the handle and free it using
 *               pumpio_piubtn_usb_close.
 * @return Success or an error code as defined by result_t. Possible return
 *         values: RESULT_SUCCESS, EIO, EINVAL, EACCES, ENODEV, ENOENT, EBUSY,
 *         EAGAIN, EOVERFLOW, EPIPE, EINTR, ENOMEM, ENOTSUP
 */
result_t piubtn_usb_open(void **handle);

/**
 * Run a one synchronous polling call setting outputs and getting inputs.
 * 
 * @param handle Valid handle of an opened PIUBTN usb device
 * @param output Pointer to an allocated buffer with the output data to send.
 * @param input Pointer to an allocated buffer for the input data to receive.
 * @return Success or an error code as defined by result_t. Possible return
 *         values: RESULT_SUCCESS, EIO, EINVAL, EACCES, ENODEV, ENOENT, EBUSY,
 *         EAGAIN, EOVERFLOW, EPIPE, EINTR, ENOMEM, ENOTSUP
 */
result_t piubtn_usb_poll(
    void *handle,
    const union piubtn_output_paket *output,
    union piubtn_input_paket *input);

/**
 * Close an opened PIUBTN usb device.
 * 
 * Ensure you call this for every PIUBTN device opened to free resources.
 *
 * @param handle Valid handle of the opened PIUBTN device to close
 */
void piubtn_usb_close(void *handle);

#endif