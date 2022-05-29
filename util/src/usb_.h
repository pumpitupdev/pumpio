/**
 * Wrapper module for libusb-1.0 to make it more convenient to use usb devices
 */
#ifndef PUMPIO_USB_H
#define PUMPIO_USB_H

#include <stdbool.h>
#include <stdint.h>

#include "result.h"

/**
 * Check if a USB device with the given VID and PID is connected and can be
 * opened.
 *
 * @param vid Vendor ID of the USB device to check
 * @param pid Product ID of the USB device to check
 * @return True if device is connected, false otherwise.
 */
bool pumpio_usb_available(uint16_t vid, uint16_t pid);

/**
 * Open a usb device with given parameters.
 *
 * @param handle Pointer to variable (void*) to store the resulting handle
 *               reference in if successful. The caller is responsible for
 *               managing the handle and free it using pumpio_usb_close.
 * @param vid Vendor ID of the device
 * @param pid Product ID of the device
 * @param config Configuration to set when opening the device
 * @param iface Interface to claim
 * @return Success or an error code as defined by result_t. Possible return
 *         values: RESULT_SUCCESS, EIO, EINVAL, EACCES, ENODEV, ENOENT, EBUSY,
 *         EAGAIN, EOVERFLOW, EPIPE, EINTR, ENOMEM, ENOTSUP
 */
result_t pumpio_usb_open(
    void **handle, uint16_t vid, uint16_t pid, uint16_t config, uint16_t iface);

/**
 * Execute a synchronous control transfer from the host to the device.
 *
 * @param handle Valid handle of an opened usb device
 * @param request_type Request type field for the setup packet
 * @param request Request field for the setup packet
 * @param value Value field for the setup packet
 * @param index Index field for the setup packet
 * @param data Pointer to an allocated buffer or vendor defined length for
 *             either input or output depending on the direction bits set by
 *             request_type
 * @param len Length of the allocated data buffer provided in data
 * @param timeout_ms Timeout for the operation in ms. This call blocks for at
 *                   least the given amount of time until it aborts the
 *                   operation if not successfully executed. Set value 0 for
 *                   unlimited timeout (not recommended).
 * @param len_res Pointer to a field to return the resulting number of
 *                successfully transfered bytes to.
 * @return Success or an error code as defined by result_t. Possible return
 *         values: RESULT_SUCCESS, EIO, EINVAL, EACCES, ENODEV, ENOENT, EBUSY,
 *         EAGAIN, EOVERFLOW, EPIPE, EINTR, ENOMEM, ENOTSUP
 */
result_t pumpio_usb_control_transfer(
    void *handle,
    uint8_t request_type,
    uint8_t request,
    uint16_t value,
    uint16_t index,
    uint8_t *data,
    uint16_t len,
    uint32_t timeout_ms,
    uint16_t *len_res);

/**
 * Close an opened usb device.
 *
 * Ensure you call this for every device opened to free resources.
 *
 * @param handle Valid handle of the opened device to close
 */
void pumpio_usb_close(void *handle);

#endif