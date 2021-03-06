/*
 * This file is part of the coreboot project.
 *
 * Copyright 2003 Tyan
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "amd8111.h"

static void amd8111_usb2_enable(struct device *dev)
{
	// Due to buggy USB2 we force it to disable.
	dev->enabled = 0;
	amd8111_enable(dev);
	printk(BIOS_DEBUG, "USB2 disabled.\n");
}

static struct device_operations usb2_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.scan_bus         = 0,
	.enable           = amd8111_usb2_enable,
};

static const struct pci_driver usb2_driver __pci_driver = {
	.ops    = &usb2_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_8111_USB2,
};
