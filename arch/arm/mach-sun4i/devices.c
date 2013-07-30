/*
 * arch/arch/mach-sun4i/devices.c
 * (C) Copyright 2010-2015
 * Reuuimlla Technology Co., Ltd. <www.reuuimllatech.com>
 * Benn Huang <benn@reuuimllatech.com>
 *
 * SUN4I platform devices
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/serial_8250.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/pda_power.h>
#include <linux/io.h>
#include <linux/i2c.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/setup.h>
#include <mach/hardware.h>
#include <mach/i2c.h>
#if(CONFIG_CPU_HAS_PMU)
#include <asm/pmu.h>
#endif


/* uart */
static struct plat_serial8250_port debug_uart_platform_data[] = {
	{
		.membase	= (void __iomem *)SW_VA_UART0_IO_BASE,
		.mapbase	= (resource_size_t)SW_PA_UART0_IO_BASE,
		.irq		= SW_INT_IRQNO_UART0,
		.flags		= UPF_BOOT_AUTOCONF,
		.iotype		= UPIO_DWAPB32,
		.regshift	= 2,
		.uartclk	= 24000000,
	}, {
		.flags		= 0
	}
};

static struct platform_device debug_uart = {
	.name = "serial8250",
	.id = PLAT8250_DEV_PLATFORM,
	.dev = {
		.platform_data = debug_uart_platform_data,
	},
};

/* dma */
static struct platform_device sw_pdev_dmac = {
	.name = "sw_dmac",
};

static struct resource sw_res_nand =
{
	.start = SW_PA_NANDFLASHC_IO_BASE,
	.end = SW_PA_NANDFLASHC_IO_BASE + 0x1000,
	.flags = IORESOURCE_MEM,
};

struct platform_device sw_pdev_nand =
{
	.name = "sw_nand",
	.id = -1,
	.num_resources = 1,
	.resource = &sw_res_nand,
	.dev = {}
};

/* twi0 */
static struct sun4i_i2c_platform_data sun4i_twi0_pdata[] = {
	{
		.bus_num   = 0,
		.frequency = I2C0_TRANSFER_SPEED,
	},
};

static struct resource sun4i_twi0_resources[] = {
	{
		.start	= TWI0_BASE_ADDR_START,
		.end	= TWI0_BASE_ADDR_END,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= SW_INT_IRQNO_TWI0,
		.end	= SW_INT_IRQNO_TWI0,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device sun4i_twi0_device = {
	.name		= "sun4i-i2c",
	.id		    = 0,
	.resource	= sun4i_twi0_resources,
	.num_resources	= ARRAY_SIZE(sun4i_twi0_resources),
	.dev = {
		.platform_data = sun4i_twi0_pdata,
	},
};

/* twi1 */
static struct sun4i_i2c_platform_data sun4i_twi1_pdata[] = {
	{
		.bus_num   = 1,
    	.frequency = I2C1_TRANSFER_SPEED,
	},
};

static struct resource sun4i_twi1_resources[] = {
	{
		.start	= TWI1_BASE_ADDR_START,
		.end	= TWI1_BASE_ADDR_END,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= SW_INT_IRQNO_TWI1,
		.end	= SW_INT_IRQNO_TWI1,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device sun4i_twi1_device = {
	.name		= "sun4i-i2c",
	.id		    = 1,
	.resource	= sun4i_twi1_resources,
	.num_resources	= ARRAY_SIZE(sun4i_twi1_resources),
	.dev = {
		.platform_data = sun4i_twi1_pdata,
	},
};

/* twi2 */
static struct sun4i_i2c_platform_data sun4i_twi2_pdata[] = {
	{
		.bus_num   = 2,
    	.frequency = I2C2_TRANSFER_SPEED,
	},
};

static struct resource sun4i_twi2_resources[] = {
	{
		.start	= TWI2_BASE_ADDR_START,
		.end	= TWI2_BASE_ADDR_END,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= SW_INT_IRQNO_TWI2,
		.end	= SW_INT_IRQNO_TWI2,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device sun4i_twi2_device = {
	.name		= "sun4i-i2c",
	.id		    = 2,
	.resource	= sun4i_twi2_resources,
	.num_resources	= ARRAY_SIZE(sun4i_twi2_resources),
	.dev = {
		.platform_data = sun4i_twi2_pdata,
	},
};


#if(CONFIG_CPU_HAS_PMU)
/* cpu performance support */
static struct resource sun4i_pmu_resource = {
    .start	= SW_INT_IRQNO_PLE_PFM,
	.end	= SW_INT_IRQNO_PLE_PFM,
	.flags	= IORESOURCE_IRQ,
};

static struct platform_device sun4i_pmu_device = {
	.name	= "arm-pmu",
	.id		= ARM_PMU_DEVICE_CPU,
	.num_resources	= 1,
	.resource = &sun4i_pmu_resource,
};
#endif


static struct platform_device *sw_pdevs[] __initdata = {
	&debug_uart,
	&sw_pdev_dmac,
	&sw_pdev_nand,
	&sun4i_twi0_device,
	&sun4i_twi1_device,
	&sun4i_twi2_device,
    #if(CONFIG_CPU_HAS_PMU)
    &sun4i_pmu_device,
    #endif
};

void __init sw_pdev_init(void)
{
	platform_add_devices(sw_pdevs, ARRAY_SIZE(sw_pdevs));
}
