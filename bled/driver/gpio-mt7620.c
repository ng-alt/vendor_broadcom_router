/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Copyright 2014, ASUSTeK Inc.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND ASUS GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#include "../bled_defs.h"
#include "gpio_api.h"

/* copy from rt_mmap.h, ralink_gpio.h */
#define RALINK_PIO_BASE			0xB0000600
#define RALINK_PRGIO_ADDR		RALINK_PIO_BASE // Programmable I/O
#define RALINK_REG_PIODATA		(RALINK_PRGIO_ADDR + 0x20)
#define RALINK_REG_PIOSET		(RALINK_PRGIO_ADDR + 0x2C)
#define RALINK_REG_PIORESET		(RALINK_PRGIO_ADDR + 0x30)

/* ralink_gpio.h RALINK_GPIO_HAS_7224 */
#define RALINK_REG_PIO3924DATA		(RALINK_PRGIO_ADDR + 0x48)
#define RALINK_REG_PIO3924SET		(RALINK_PRGIO_ADDR + 0x54)
#define RALINK_REG_PIO3924RESET		(RALINK_PRGIO_ADDR + 0x58)
#define RALINK_REG_PIO7140DATA		(RALINK_PRGIO_ADDR + 0x70)
#define RALINK_REG_PIO7140SET		(RALINK_PRGIO_ADDR + 0x7C)
#define RALINK_REG_PIO7140RESET		(RALINK_PRGIO_ADDR + 0x80)

static struct mt7620_gpio_reg_s {
	int start;
	int end;
	unsigned int reg_data;
	unsigned int reg_set;
	unsigned int reg_clear;
} mt7620_gpio_reg_tbl[] = {
	{ 0,	23, RALINK_REG_PIODATA, RALINK_REG_PIOSET, RALINK_REG_PIORESET },
	{ 24,	39, RALINK_REG_PIO3924DATA, RALINK_REG_PIO3924SET, RALINK_REG_PIO3924RESET },
	{ 40,	71, RALINK_REG_PIO7140DATA, RALINK_REG_PIO7140SET, RALINK_REG_PIO7140RESET },

	{ -1, -1, 0, 0 },
};

/**
 * Get set/clear register base on GPIO# and value.
 * @gpio_nr:
 * @value:
 * 	> 0:	return set register
 * 	= 0:	return clear register
 * 	< 0:	return data register
 * @return:
 * 	0:	success.
 *  otherwise:	not found
 */
static int get_gpio_register(int gpio_nr, int value, u32 *reg, u32 *mask)
{
	int ret = -2;
	struct mt7620_gpio_reg_s *p;

	if (!reg || !mask)
		return -1;
	for (p = &mt7620_gpio_reg_tbl[0]; p->start >= 0; ++p) {
		if (gpio_nr < p->start || gpio_nr > p->end)
			continue;

		if (value > 0)
			*reg = p->reg_set;
		else if (value == 0)
			*reg = p->reg_clear;
		else
			*reg = p->reg_data;

		*mask = 1U << (gpio_nr - p->start);
		
		ret = 0;
	}

	return ret;
}

static void mt7620_gpio_set(int gpio_nr, int value)
{
	u32 reg = 0, mask = 0;
	if (get_gpio_register(gpio_nr, value, &reg, &mask)) {
		return;
	}
	*(volatile u32*)reg = mask;
}

static int mt7620_gpio_get(int gpio_nr)
{
	u32 reg = 0, mask = 0;
	if (get_gpio_register(gpio_nr, -1, &reg, &mask)) {
		return 0;
	}
	return !!(*(volatile u32*)reg & mask);
}

struct gpio_api_s gpio_api_tbl[GPIO_API_MAX] = {
	[GPIO_API_PLATFORM] =
	{
		.gpio_set = mt7620_gpio_set,
		.gpio_get = mt7620_gpio_get
	},
};
