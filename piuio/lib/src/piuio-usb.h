/**
 * Device level abstraction for opening, polling and closing a PIUIO usb device
 * backed by the user-space USB library libusb.
 */
#ifndef PIUIO_USB_H_
#define PIUIO_USB_H_

#include <stdbool.h>
#include <stdint.h>

#include "piuio.h"
#include "result.h"

/**
 * Check if a PIUIO device is connected via USB and available to be opened.
 *
 * @return True if device is connected, false otherwise.
 */
bool piuio_usb_available();

/**
 * Open a connected PIUIO device using a user space usb library.
 *
 * @param handle Pointer to variable (void*) to store the resulting handle
 *               reference in if the call is successful. The caller is
 *               responsible for managing the handle and free it using
 *               piuio_usb_close.
 * @return Success or an error code as defined by result_t. Possible return
 *         values: RESULT_SUCCESS, EIO, EINVAL, EACCES, ENODEV, ENOENT, EBUSY,
 *         EAGAIN, EOVERFLOW, EPIPE, EINTR, ENOMEM, ENOTSUP
 */
result_t piuio_usb_open(void **handle);

/**
 * Run a single set output, get input poll cycle.
 *
 * This results in querying only a single selected sensor out of all four
 * sensors. In order to get data from all four sensors, either cycle the bit
 * mask in the output paket or use piuio_usb_poll_full_cycle for
 * convenience.
 *
 * @param handle Valid handle of an opened PIUIO usb device
 * @param output Pointer to an allocated buffer with the output data to send.
 * @param input Pointer to an allocated buffer for the input data to receive.
 * @return Success or an error code as defined by result_t. Possible return
 *         values: RESULT_SUCCESS, EIO, EINVAL, EACCES, ENODEV, ENOENT, EBUSY,
 *         EAGAIN, EOVERFLOW, EPIPE, EINTR, ENOMEM, ENOTSUP
 */
result_t piuio_usb_poll_one_cycle(
    void *handle,
    const union piuio_output_paket *output,
    union piuio_input_paket *input);

/**
 * Execute a full polling cycle which consists of four calls to set outputs
 * and four calls to get inputs.
 *
 * As each panel of the stage has four sensors which are multiplexed over the
 * protocol, one input paket can only contain data for a single sensor. We need
 * to issue four output queries to select each sensor and four input queries to
 * get the data for each selected sensor.
 *
 * @param handle Valid handle of an opened PIUIO usb device
 * @param output Pointer to an allocated buffer with the output data to send.
 * @param input Pointer to an allocated buffer for the batched input data to
 *              receive.
 * @return Success or an error code as defined by result_t. Possible return
 *         values: RESULT_SUCCESS, EIO, EINVAL, EACCES, ENODEV, ENOENT, EBUSY,
 *         EAGAIN, EOVERFLOW, EPIPE, EINTR, ENOMEM, ENOTSUP
 */
result_t piuio_usb_poll_full_cycle(
    void *handle,
    union piuio_output_paket *output,
    struct piuio_usb_input_batch_paket *input);

/**
 * Close an opened PIUIO usb device.
 *
 * Ensure you call this for every PIUIO device opened to free resources.
 *
 * @param handle Valid handle of the opened PIUIO device to close
 */
void piuio_usb_close(void *handle);

#endif