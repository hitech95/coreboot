/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corporation.
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
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#define CACHE_LINE_SIZE	0x10
/* Latency tolerance reporting, max non-snoop latency value 3.14ms */
#define PCIE_LTR_MAX_NO_SNOOP_LATENCY_VALUE	0x1003
/* Latency tolerance reporting, max snoop latency value 3.14ms */
#define PCIE_LTR_MAX_SNOOP_LATENCY_VALUE	0x1003
/* PCI-E Sub-System ID */
#define PCIE_SUBSYSTEM_VENDOR_ID	0x94

static void pch_pcie_init(struct device *dev)
{
	u16 reg16;

	printk(BIOS_DEBUG, "Initializing PCH PCIe bridge.\n");

	/* Enable SERR */
	pci_or_config32(dev, PCI_COMMAND, PCI_COMMAND_SERR);

	/* Enable Bus Master */
	pci_or_config32(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* Set Cache Line Size to 0x10 */
	pci_write_config8(dev, PCI_CACHE_LINE_SIZE, CACHE_LINE_SIZE);

	/* disable parity error response, enable ISA */
	pci_update_config16(dev, PCI_BRIDGE_CONTROL, ~1, 1<<2);

	if (IS_ENABLED(CONFIG_PCIE_DEBUG_INFO)) {
		printk(BIOS_SPEW, "    MBL    = 0x%08x\n",
				pci_read_config32(dev, PCI_MEMORY_BASE));
		printk(BIOS_SPEW, "    PMBL   = 0x%08x\n",
				pci_read_config32(dev, PCI_PREF_MEMORY_BASE));
		printk(BIOS_SPEW, "    PMBU32 = 0x%08x\n",
				pci_read_config32(dev, PCI_PREF_BASE_UPPER32));
		printk(BIOS_SPEW, "    PMLU32 = 0x%08x\n",
				pci_read_config32(dev, PCI_PREF_LIMIT_UPPER32));
	}

	/* Clear errors in status registers */
	reg16 = pci_read_config16(dev, PCI_STATUS);
	pci_write_config16(dev, PCI_STATUS, reg16);
	reg16 = pci_read_config16(dev, PCI_SEC_STATUS);
	pci_write_config16(dev, PCI_SEC_STATUS, reg16);
}

static void pcie_set_L1_ss_max_latency(struct device *dev, unsigned int offset)
{
	/* Set max snoop and non-snoop latency for the SOC */
	pci_write_config32(dev, offset,
			PCIE_LTR_MAX_NO_SNOOP_LATENCY_VALUE << 16 |
			PCIE_LTR_MAX_SNOOP_LATENCY_VALUE);
}

static void pcie_dev_set_subsystem(struct device *dev,
		unsigned vendor, unsigned device)
{
	pci_write_config32(dev, PCIE_SUBSYSTEM_VENDOR_ID,
			((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations pcie_ops = {
	.set_L1_ss_latency = pcie_set_L1_ss_max_latency,
	.set_subsystem = pcie_dev_set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources		= pci_bus_enable_resources,
	.init		= pch_pcie_init,
	.scan_bus		= pciexp_scan_bridge,
	.ops_pci		= &pcie_ops,
};

static const unsigned short pcie_device_ids[] = {
	PCI_DEVICE_ID_INTEL_SPT_LP_PCIE_RP1,
	PCI_DEVICE_ID_INTEL_SPT_LP_PCIE_RP2,
	PCI_DEVICE_ID_INTEL_SPT_LP_PCIE_RP3,
	PCI_DEVICE_ID_INTEL_SPT_LP_PCIE_RP4,
	PCI_DEVICE_ID_INTEL_SPT_LP_PCIE_RP5,
	PCI_DEVICE_ID_INTEL_SPT_LP_PCIE_RP6,
	PCI_DEVICE_ID_INTEL_SPT_LP_PCIE_RP7,
	PCI_DEVICE_ID_INTEL_SPT_LP_PCIE_RP8,
	PCI_DEVICE_ID_INTEL_SPT_LP_PCIE_RP9,
	PCI_DEVICE_ID_INTEL_SPT_LP_PCIE_RP10,
	PCI_DEVICE_ID_INTEL_SPT_LP_PCIE_RP11,
	PCI_DEVICE_ID_INTEL_SPT_LP_PCIE_RP12,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP1,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP2,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP3,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP4,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP5,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP6,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP7,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP8,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP9,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP10,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP11,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP12,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP13,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP14,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP15,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP16,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP17,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP18,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP19,
	PCI_DEVICE_ID_INTEL_SPT_H_PCIE_RP20,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP1,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP2,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP3,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP4,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP5,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP6,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP7,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP8,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP9,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP10,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP11,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP12,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP13,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP14,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP15,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP16,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP17,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP18,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP19,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP20,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP21,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP22,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP23,
	PCI_DEVICE_ID_INTEL_KBP_H_PCIE_RP24,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP1,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP2,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP3,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP4,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP5,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP6,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP7,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP8,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP9,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP10,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP11,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP12,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP13,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP14,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP15,
	PCI_DEVICE_ID_INTEL_CNL_LP_PCIE_RP16,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP1,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP2,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP3,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP4,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP5,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP6,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP7,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP8,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP9,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP10,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP11,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP12,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP13,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP14,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP15,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP16,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP17,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP18,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP19,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP20,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP21,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP22,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP23,
	PCI_DEVICE_ID_INTEL_CNP_H_PCIE_RP24,
	0
};

static const struct pci_driver pch_pcie __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.devices	= pcie_device_ids,
};
