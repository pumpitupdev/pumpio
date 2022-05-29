#include <assert.h>
#include <errno.h>
#include <libusb-1.0/libusb.h>
#include <stdlib.h>

#include "usb_.h"

struct pumpio_usb_ctx {
  struct libusb_context *ctx;
  struct libusb_device_handle *dev;
};

static result_t pumpio_usb_map_libusb_error(int32_t libusb_error)
{
  switch (libusb_error) {
    case LIBUSB_SUCCESS:
      return RESULT_SUCCESS;
    case LIBUSB_ERROR_IO:
      return EIO;
    case LIBUSB_ERROR_INVALID_PARAM:
      return EINVAL;
    case LIBUSB_ERROR_ACCESS:
      return EACCES;
    case LIBUSB_ERROR_NO_DEVICE:
      return ENODEV;
    case LIBUSB_ERROR_NOT_FOUND:
      return ENOENT;
    case LIBUSB_ERROR_BUSY:
      return EBUSY;
    case LIBUSB_ERROR_TIMEOUT:
      return EAGAIN;
    case LIBUSB_ERROR_OVERFLOW:
      return EOVERFLOW;
    case LIBUSB_ERROR_PIPE:
      return EPIPE;
    case LIBUSB_ERROR_INTERRUPTED:
      return EINTR;
    case LIBUSB_ERROR_NO_MEM:
      return ENOMEM;
    case LIBUSB_ERROR_NOT_SUPPORTED:
      return ENOTSUP;
    case LIBUSB_ERROR_OTHER:
      // fallthrough
    default:
      return 999;
  }
}

static int32_t pumpio_usb_get_device_handle(
    struct libusb_device_handle **handle,
    struct libusb_context *ctx,
    uint16_t vid,
    uint16_t pid)
{
  int32_t ret;
  struct libusb_device **dev_list;
  struct libusb_device *device;

  dev_list = NULL;
  device = NULL;

  assert(handle != NULL);
  assert(ctx != NULL);

  ssize_t num_dev = libusb_get_device_list(ctx, &dev_list);

  /* find device */
  for (ssize_t i = 0; i < num_dev; i++) {
    struct libusb_device_descriptor dev_desc_tmp;
    struct libusb_device *dev_tmp;

    dev_tmp = dev_list[i];

    libusb_get_device_descriptor(dev_tmp, &dev_desc_tmp);

    if (dev_desc_tmp.idVendor == vid && dev_desc_tmp.idProduct == pid) {
      device = dev_tmp;
      break;
    }
  }

  /* not found */
  if (device == NULL) {
    libusb_free_device_list(dev_list, 1);
    return LIBUSB_ERROR_NOT_FOUND;
  }

  ret = libusb_open(device, handle);

  if (ret != LIBUSB_SUCCESS) {
    libusb_free_device_list(dev_list, 1);

    return ret;
  }

  libusb_free_device_list(dev_list, 1);

  return LIBUSB_SUCCESS;
}

bool pumpio_usb_available(uint16_t vid, uint16_t pid)
{
  bool found;
  int32_t ret;
  struct libusb_context *ctx;
  struct libusb_device **dev_list;
  struct libusb_device *device;

  dev_list = NULL;
  device = NULL;

  ret = libusb_init(&ctx);

  if (ret != LIBUSB_SUCCESS) {
    return false;
  }

  ssize_t num_dev = libusb_get_device_list(ctx, &dev_list);

  /* find device */
  for (ssize_t i = 0; i < num_dev; i++) {
    struct libusb_device_descriptor dev_desc_tmp;
    struct libusb_device *dev_tmp;

    dev_tmp = dev_list[i];

    libusb_get_device_descriptor(dev_tmp, &dev_desc_tmp);

    if (dev_desc_tmp.idVendor == vid && dev_desc_tmp.idProduct == pid) {
      device = dev_tmp;
      break;
    }
  }

  found = device != NULL;

  libusb_free_device_list(dev_list, 1);
  libusb_exit(ctx);

  return found;
}

result_t pumpio_usb_open(
    void **handle, uint16_t vid, uint16_t pid, uint16_t config, uint16_t iface)
{
  int32_t ret;
  struct libusb_context *ctx;
  struct libusb_device_handle *dev;
  struct pumpio_usb_ctx *handle_tmp;

  assert(handle);

  ret = libusb_init(&ctx);

  if (ret != LIBUSB_SUCCESS) {
    return pumpio_usb_map_libusb_error(ret);
  }

  /* dont print any error messages to stderr, change this for debugging */
  // libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_NONE);

  ret = pumpio_usb_get_device_handle(&dev, ctx, vid, pid);

  if (ret != LIBUSB_SUCCESS) {
    libusb_exit(ctx);
    return pumpio_usb_map_libusb_error(ret);
  }

  /* check if the device is attached to the kernel, detach it first then */
  if (libusb_kernel_driver_active(dev, iface) == 1) {
    ret = libusb_detach_kernel_driver(dev, iface);

    if (ret != LIBUSB_SUCCESS) {
      libusb_close(dev);
      libusb_exit(ctx);

      return pumpio_usb_map_libusb_error(ret);
    }
  }

  ret = libusb_set_configuration(dev, config);

  if (ret != LIBUSB_SUCCESS) {
    libusb_close(dev);
    libusb_exit(ctx);

    return pumpio_usb_map_libusb_error(ret);
  }

  ret = libusb_claim_interface(dev, iface);

  if (ret != LIBUSB_SUCCESS) {
    libusb_close(dev);
    libusb_exit(ctx);

    return pumpio_usb_map_libusb_error(ret);
  }

  ret = libusb_set_interface_alt_setting(dev, iface, 0);

  if (ret != LIBUSB_SUCCESS) {
    libusb_close(dev);
    libusb_exit(ctx);

    return pumpio_usb_map_libusb_error(ret);
  }

  handle_tmp = (struct pumpio_usb_ctx *) malloc(sizeof(struct pumpio_usb_ctx));

  handle_tmp->ctx = ctx;
  handle_tmp->dev = dev;

  (*handle) = (void *) handle_tmp;

  return RESULT_SUCCESS;
}

result_t pumpio_usb_control_transfer(
    void *handle,
    uint8_t request_type,
    uint8_t request,
    uint16_t value,
    uint16_t index,
    uint8_t *data,
    uint16_t len,
    uint32_t timeout_ms,
    uint16_t *len_res)
{
  struct pumpio_usb_ctx *dev;
  int32_t ret;

  assert(handle != NULL);
  assert(data != NULL);
  assert(len_res != NULL);

  dev = (struct pumpio_usb_ctx *) handle;

  ret = libusb_control_transfer(
      dev->dev, request_type, request, value, index, data, len, timeout_ms);

  if (ret < 0) {
    return pumpio_usb_map_libusb_error(ret);
  }

  *len_res = ret;

  return RESULT_SUCCESS;
}

void pumpio_usb_close(void *handle)
{
  assert(handle);

  struct pumpio_usb_ctx *dev = (struct pumpio_usb_ctx *) handle;

  assert(dev->dev);
  assert(dev->ctx);

  libusb_close(dev->dev);
  libusb_exit(dev->ctx);

  free(dev);
}
