/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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

#include <bootblock_common.h>
#include <ec.h>
#include <soc/southbridge.h>

void bootblock_mainboard_init(void)
{
	/* Enable the EC as soon as we have visibility */
	mainboard_ec_init();

	/* Setup TPM decode before verstage */
	sb_tpm_decode_spi();
}
