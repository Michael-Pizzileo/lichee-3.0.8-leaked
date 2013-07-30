/*
 * Nanoradio sdio wifi power management API
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <mach/sys_config.h>

#include "mmc_pm.h"

#define nano_msg(...)    do {printk("[nano]: "__VA_ARGS__);} while(0)

static int nano_powerup = 0;
static int nano_suspend = 0;

static int nano_gpio_ctrl(char* name, int level)
{
    struct mmc_pm_ops *ops = &mmc_card_pm_ops;
    char* gpio_name[4] = {"swl_n20_host_wakeup", "swl_n20_shdn",
                               "swl_n20_vcc_en", "swl_n20_vdd_en"};
    
    int i = 0;
    int ret = 0;
    
    for (i=0; i<4; i++) {
        if (strcmp(name, gpio_name[i])==0)
            break;
    }
    if (i==4) {
        nano_msg("No gpio %s for nano-wifi module\n", name);
        return -1;
    }
    
    ret = gpio_write_one_pin_value(ops->pio_hdle, level, name);
    if (ret) {
        nano_msg("Failed to set gpio %s to %d !\n", name, level);
        return -1;
    }
    if (strcmp(name, "swl_n20_vdd_en")==0) {
        nano_powerup = level;
        nano_msg("Wifi Power %s !!\n", level ? "UP" : "Off");
    }
    return 0;
}

static int nano_get_io_value(char* name)
{
    struct mmc_pm_ops *ops = &mmc_card_pm_ops;
    char* hostwake = "swl_n20_host_wakeup";
    
    if (strcmp(name, hostwake)) {
        nano_msg("No gpio %s for swl-n20\n", name);
        return -1;
    }
    
    return gpio_read_one_pin_value(ops->pio_hdle, name);
}

static void nano_standby(int instadby)
{
    if (instadby) {
        if (nano_powerup) {
            nano_gpio_ctrl("swl_n20_shdn", 0);
            nano_gpio_ctrl("swl_n20_vdd_en", 0);
            nano_gpio_ctrl("swl_n20_vcc_en", 0);
            nano_suspend = 1;
        }
    } else {
        if (nano_suspend) {
            nano_gpio_ctrl("swl_n20_vcc_en", 1);
            udelay(100);
            nano_gpio_ctrl("swl_n20_shdn", 1);
            udelay(50);
            nano_gpio_ctrl("swl_n20_vdd_en", 1);
            sunximmc_rescan_card(3, 1);
            nano_suspend = 0;
        }
    }
}

static void nano_power(int mode, int* updown)
{
	if (mode) {
		if (*updown) {
            nano_gpio_ctrl("swl_n20_vcc_en", 1);
            udelay(100);
            nano_gpio_ctrl("swl_n20_shdn", 1);
            udelay(50);
            nano_gpio_ctrl("swl_n20_vdd_en", 1);
            udelay(50);
		} else {
            nano_gpio_ctrl("swl_n20_shdn", 0);
            nano_gpio_ctrl("swl_n20_vdd_en", 0);
            nano_gpio_ctrl("swl_n20_vcc_en", 0);
		}
	} else {
        if (nano_powerup)
            *updown = 1;
        else
            *updown = 0;
		nano_msg("sdio wifi power state: %s\n", nano_powerup ? "on" : "off");
	}
}

void nano_wifi_gpio_init(void)
{
    struct mmc_pm_ops *ops = &mmc_card_pm_ops;
    nano_powerup = 0;
    nano_suspend = 0;
    ops->gpio_ctrl = nano_gpio_ctrl;
    ops->get_io_val = nano_get_io_value;
    ops->standby = nano_standby;
    ops->power = nano_power;
}