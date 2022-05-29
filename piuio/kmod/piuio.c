/*
 * Based on version 0.1 of djpohly's piuio kernel module:
 * https://github.com/djpohly/piuio
 * 
 * PIUIO interface driver
 *
 * Copyright (C) 2012-2014 Devin J. Pohly (djpohly+linux@gmail.com)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as
 *	published by the Free Software Foundation, version 2.
 *
 * This code is based on the USB skeleton driver by Greg Kroah-Hartman.
 */
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kref.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/usb.h>

// -------------------------------------------------------------------------

/* Module and driver info */
MODULE_AUTHOR("Devin J. Pohly & icex2");
MODULE_DESCRIPTION("PIUIO input/output driver");
MODULE_LICENSE("GPL");

/* Module parameters */
static int timeout_ms = 10;
module_param(timeout_ms, int, 0644);
MODULE_PARM_DESC(
    timeout_ms,
    "Timeout for PIUIO USB messages in ms"
    " (default 10)");

// -------------------------------------------------------------------------

static int piuio_open(struct inode *inode, struct file *filp);
static ssize_t
piuio_read(struct file *filp, char __user *ubuf, size_t sz, loff_t *pofs);
static int piuio_release(struct inode *inode, struct file *filp);

/* File operations for /dev/piuioN */
static const struct file_operations piuio_fops = {
    .owner = THIS_MODULE,
    .open = piuio_open,
    .read = piuio_read,
    .release = piuio_release,
};

// -------------------------------------------------------------------------

/* Vendor/product ID table */
static const struct usb_device_id piuio_ids[] = {
    {USB_DEVICE(0x0547, 0x1002)},
    {},
};
MODULE_DEVICE_TABLE(usb, piuio_ids);

/* Class driver, for creating device files */
static struct usb_class_driver piuio_class = {
    .name = "piuio%d",
    .fops = &piuio_fops,
};

static int
piuio_probe(struct usb_interface *intf, const struct usb_device_id *id);
static void piuio_disconnect(struct usb_interface *intf);

/* Device driver handlers */
static struct usb_driver piuio_driver = {
    .name = "piuio",
    .probe = piuio_probe,
    .disconnect = piuio_disconnect,
    .id_table = piuio_ids,
    .supports_autosuspend = 1,
};

static struct usb_driver piuio_driver;

// -------------------------------------------------------------------------

/* Protocol-specific parameters */
#define PIUIO_MSG_REQ 0xAE
#define PIUIO_MSG_VAL 0x00
#define PIUIO_MSG_IDX 0x00

/* Size of input and output packets */
#define PIUIO_INPUT_PACKET_SIZE 8
#define PIUIO_OUTPUT_PACKET_SIZE 8
#define PIUIO_INPUT_MULTIPLEX_NUM 4

// -------------------------------------------------------------------------

/* Represents the current state of an interface */
struct piuio_state {
  /* USB device and interface */
  struct usb_device *dev;
  struct usb_interface *intf;
  /* Concurrency control */
  struct mutex lock;
  struct kref kref;
  // Native data buffers for reading/writing device
  // Buffers must be managed here and heap allocate in the kernel
  // Otherwise, usb_control_msg fails with resource temporary unavailable
  // (EAGAIN)
  unsigned char outputs[PIUIO_OUTPUT_PACKET_SIZE];
  unsigned char inputs[PIUIO_INPUT_PACKET_SIZE * PIUIO_INPUT_MULTIPLEX_NUM];
};

// -------------------------------------------------------------------------

/**
 *  Auxiliary function to clean up interface state
 */
static void piuio_free(struct kref *kref)
{
  struct piuio_state *st = container_of(kref, struct piuio_state, kref);

  usb_put_dev(st->dev);
  kfree(st);
}

// -------------------------------------------------------------------------

/**
 * Open the device. Issued on open() call.
 */
static int piuio_open(struct inode *inode, struct file *filp)
{
  struct usb_interface *intf;
  struct piuio_state *st;
  int result;

  /* Get the corresponding interface and state */
  intf = usb_find_interface(&piuio_driver, iminor(inode));

  if (!intf) {
    return -ENODEV;
  }

  st = usb_get_intfdata(intf);

  if (!st) {
    return -ENODEV;
  }

  /* Pick up a reference to the interface */
  kref_get(&st->kref);

  /* Ensure the device isn't suspended while in use */
  result = usb_autopm_get_interface(intf);

  if (result) {
    kref_put(&st->kref, piuio_free);
    return result;
  }

  /* Attach our state to the file */
  filp->private_data = st;

  return 0;
}

/**
 * Single read call to write the current output state (lights) as well as
 * fetch a full input update cycle of all sensores. This call expects the
 * output lights data to be in the first 8 bytes of the buffer.
 * The buffer is fully populated with 4x input data (multiplexed sensores).
 */
static ssize_t
piuio_read(struct file *filp, char __user *ubuf, size_t sz, loff_t *pofs)
{
  struct piuio_state *st;
  int i;
  int result = 0;

  st = filp->private_data;

  mutex_lock(&st->lock);

  /* Device closed */
  if (!st->intf) {
    result = -ENODEV;
    goto out;
  }

  /* Transfer user space buffered outputs to kernel buffer, required */
  if (copy_from_user(st->outputs, ubuf, sizeof(st->outputs))) {
    result = -EFAULT;
    goto out;
  }

  /* Run a full update cycle */
  for (i = 0; i < PIUIO_INPUT_MULTIPLEX_NUM; i++) {
    /* Select set of sensores for next inputs to fetch */
    st->outputs[0] = (st->outputs[0] & ~0x03) | i;
    st->outputs[2] = (st->outputs[2] & ~0x03) | i;

    /* Sets current light outputs and sensor mask */
    result = usb_control_msg(
        st->dev,
        usb_sndctrlpipe(st->dev, 0),
        PIUIO_MSG_REQ,
        USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
        PIUIO_MSG_VAL,
        PIUIO_MSG_IDX,
        &st->outputs,
        sizeof(st->outputs),
        timeout_ms);

    if (result < 0) {
      goto out;
    }

    /* Get inputs selected by sensor mask */
    result = usb_control_msg(
        st->dev,
        usb_rcvctrlpipe(st->dev, 0),
        PIUIO_MSG_REQ,
        USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
        PIUIO_MSG_VAL,
        PIUIO_MSG_IDX,
        &st->inputs[i * PIUIO_INPUT_PACKET_SIZE],
        PIUIO_INPUT_PACKET_SIZE,
        timeout_ms);

    if (result < 0) {
      goto out;
    }
  }

  if (copy_to_user(ubuf, st->inputs, sizeof(st->inputs))) {
    result = -EFAULT;
    goto out;
  }

out:
  mutex_unlock(&st->lock);

  if (result < 0) {
    return result;
  } else {
    return sizeof(st->inputs);
  }
}

/**
 * Cleans up after the last close() on a file descriptor
 */
static int piuio_release(struct inode *inode, struct file *filp)
{
  struct piuio_state *st;

  st = filp->private_data;

  if (st == NULL) {
    return -ENODEV;
  }

  if (st->intf) {
    usb_autopm_put_interface(st->intf);
  }

  /* Drop reference */
  kref_put(&st->kref, piuio_free);

  return 0;
}

// -------------------------------------------------------------------------

/**
 * Set up a device being connected to this driver
 */
static int
piuio_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
  struct piuio_state *st;
  int result;

  /* Set up state structure */
  st = kzalloc(sizeof(*st), GFP_KERNEL);

  if (!st) {
    dev_err(&intf->dev, "Failed to allocate state\n");
    return -ENOMEM;
  }

  kref_init(&st->kref);
  mutex_init(&st->lock);

  st->dev = usb_get_dev(interface_to_usbdev(intf));
  st->intf = intf;

  /* Store a pointer so we can get at the state later */
  usb_set_intfdata(intf, st);

  /* Register the device */
  result = usb_register_dev(intf, &piuio_class);

  if (result) {
    dev_err(&intf->dev, "Failed to register device\n");
    usb_set_intfdata(intf, NULL);
    kref_put(&st->kref, piuio_free);
  }

  return result;
}

/**
 * Clean up when a device is disconnected
 */
static void piuio_disconnect(struct usb_interface *intf)
{
  struct piuio_state *st = usb_get_intfdata(intf);

  usb_set_intfdata(intf, NULL);
  usb_deregister_dev(intf, &piuio_class);

  mutex_lock(&st->lock);
  st->intf = NULL;
  mutex_unlock(&st->lock);

  kref_put(&st->kref, piuio_free);
}

// -------------------------------------------------------------------------

/**
 * Register the driver on module load
 */
static int __init piuio_init(void)
{
  return usb_register(&piuio_driver);
}

/**
 * Remove the driver on module unload
 */
static void __exit piuio_exit(void)
{
  usb_deregister(&piuio_driver);
}

module_init(piuio_init);
module_exit(piuio_exit);
