/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
 * Copyright (C) 2016 Intel Corporation
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

#include <arch/io.h>
#include <console/uart.h>
#include <device/pci_def.h>
#include <intelblocks/lpss.h>
#include <intelblocks/pcr.h>
#include <stdint.h>
#include <soc/bootblock.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <gpio.h>

/* Serial IO UART controller legacy mode */
#define PCR_SERIAL_IO_GPPRVRW7		0x618
#define PCR_SIO_PCH_LEGACY_UART0	(1 << 0)
#define PCR_SIO_PCH_LEGACY_UART1	(1 << 1)
#define PCR_SIO_PCH_LEGACY_UART2	(1 << 2)

/* Clock divider parameters for 115200 baud rate */
#define CLOCK_DIV_M_VAL	0x30
#define CLOCK_DIV_N_VAL	0xc35

/* UART2 pad configuration. Support RXD and TXD for now. */
static const struct pad_config uart2_pads[] = {
/* UART2_RXD */		PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
/* UART2_TXD */		PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
};

void pch_uart_init(void)
{
	device_t dev = PCH_DEV_UART2;
	u32 tmp;
	uintptr_t base = uart_platform_base(CONFIG_UART_FOR_CONSOLE);

	/* Set configured UART2 base address */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, (u32)base);

	/* Enable memory access and bus master */
	tmp = pci_read_config32(dev, PCI_COMMAND);
	tmp |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	pci_write_config32(dev, PCI_COMMAND, tmp);

	/* Take UART2 out of reset */
	lpss_reset_release(base);

	/*
	 * Set M and N divisor inputs and enable clock.
	 * Main reference frequency to UART is:
	 * 120MHz * M / N = 120MHz * 48 / 3125 = 1843200Hz
	 */
	lpss_clk_update(base, CLOCK_DIV_M_VAL, CLOCK_DIV_N_VAL);

	/* Put UART2 in byte access mode for 16550 compatibility */
	if (!IS_ENABLED(CONFIG_DRIVERS_UART_8250MEM_32))
		pcr_write32(PID_SERIALIO, PCR_SERIAL_IO_GPPRVRW7,
			PCR_SIO_PCH_LEGACY_UART2);

	gpio_configure_pads(uart2_pads, ARRAY_SIZE(uart2_pads));
}
