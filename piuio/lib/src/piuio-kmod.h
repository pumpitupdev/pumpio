/**
 * Device level abstraction for opening, polling and closing a PIUIO usb device
 * backed by a custom kernel module.
 */
#ifndef PIUIO_KMOD_H_
#define PIUIO_KMOD_H_

#include <stdbool.h>

#include "piuio.h"
#include "result.h"

#define PIUIO_KMOD_INPUT_PAKET_SIZE \
  (PIUIO_INPUT_PAKET_SIZE * PIUIO_SENSOR_MASK_TOTAL_COUNT)
#define PIUIO_KMOD_OUTPUT_PAKET_SIZE PIUIO_OUTPUT_PAKET_SIZE

/**
 * Data structure for a buffer defining a single paket for polling the kernel
 * module.
 *
 * Note that output and input data are overlapping.
 */
union piuio_kmod_paket {
  struct piuio_usb_input_batch_paket input;
  union piuio_output_paket output;
  uint8_t raw[PIUIO_KMOD_INPUT_PAKET_SIZE];
};

/**
 * Checks if the kernel module is loaded and the PIUIO device is connected.
 *
 * @return True if kernel module loaded and device is connected, false
 *         otherwise.
 */
bool piuio_kmod_available();

/**
 * Open a handle to the file device exposed by the kernel module.
 *
 * @param fd Pointer to a variable to store the resulting handle reference in if
 *           the vall is successful.
 * @return Success or an error code as defined by result_t.
 */
result_t piuio_kmod_open(int *fd);

/**
 * Execute a single user-space to kernel call to issue a full polling cycle
 * in the kernel module which consists of four calls to set outputs
 * and four calls to get inputs.
 *
 * As each panel of the stage has four sensors which are multiplexed over the
 * protocol, one input paket can only contain data for a single sensor. We need
 * to issue four output queries to select each sensor and four input queries to
 * get the data for each selected sensor.
 *
 * @param fd A valid and opened file handle  to the PIUIO device
 * @param output Pointer to an allocated buffer. When calling this function, the
 *               buffer should contain the output data to write. After returning
 *               successfully, it contains a full polling cycle of input data
 *               as defined by the data structure.
 * @return Success or an error code as defined by result_t. Possible return
 *         values: RESULT_SUCCESS, EIO, EINVAL, EACCES, ENODEV, ENOENT, EBUSY,
 *         EAGAIN, EOVERFLOW, EPIPE, EINTR, ENOMEM, ENOTSUP
 */
result_t piuio_kmod_poll(int fd, union piuio_kmod_paket *paket);

/**
 * Close an opened PIUIO usb device.
 *
 * Ensure you call this for every PIUIO device opened to free resources.
 *
 * @param fd Valid file handle of the opened PIUIO device to close
 */
void piuio_kmod_close(int fd);

static_assert(
    sizeof(union piuio_kmod_paket) == PIUIO_KMOD_INPUT_PAKET_SIZE,
    "Expected size of piuio_kmod_paket incorrect");

#endif