
/*
 * USI wm-bn-bm-01-5(bcm4329) sdio wifi power management API
 * evb gpio define
 * A10 gpio define:
 * usi_bm01a_wl_pwr        = port:PH12<1><default><default><0>
 * usi_bm01a_wlbt_regon    = port:PI11<1><default><default><0>
 * usi_bm01a_wl_rst        = port:PI10<1><default><default><0>
 * usi_bm01a_wl_wake       = port:PI12<1><default><default><0>
 * usi_bm01a_bt_rst        = port:PB05<1><default><default><0>
 * usi_bm01a_bt_wake       = port:PI20<1><default><default><0>
 * usi_bm01a_bt_hostwake   = port:PI21<0><default><default><0>
 * -----------------------------------------------------------
 * A12 gpio define:
 * usi_bm01a_wl_pwr        = LDO3
 * usi_bm01a_wl_wake       = port:PA01<1><default><default><0>
 * usi_bm01a_wlbt_regon    = port:PA02<1><default><default><0>
 * usi_bm01a_wl_rst        = port:PA03<1><default><default><0>
 * usi_bm01a_bt_rst        = port:PA04<1><default><default><0>
 * usi_bm01a_bt_wake       = port:PA05<1><default><default><0>
 * usi_bm01a_bt_hostwake   = 
 */
 
#include <linux/kernel.h>
#include <linux/module.h>
#include <mach/sys_config.h>
#include <linux/delay.h>

#include "mmc_pm.h"

#define usi_msg(...)    do {printk("[usi_bm01a]: "__VA_ARGS__);} while(0)
static int usi_bm01a_wl_on = 0;
static int usi_bm01a_bt_on = 0;

#if CONFIG_CHIP_ID==1125
#include <linux/regulator/consumer.h>
static int usi_bm01a_power_onoff(int onoff)
{
	struct regulator* wifi_ldo = NULL;
	static int first = 1;
	  
#ifndef CONFIG_AW_AXP
	usi_msg("AXP driver is disabled, pls check !!\n");
	return 0;
#endif

	usi_msg("usi_bm01a_power_onoff\n");
	wifi_ldo = regulator_get(NULL, "axp20_pll");
	if (!wifi_ldo)
		usi_msg("Get power regulator failed\n");
	if (first) {
		usi_msg("first time\n");
		regulator_force_disable(wifi_ldo);
		first = 0;
	}
	if (onoff) {
		usi_msg("regulator on\n");
		regulator_set_voltage(wifi_ldo, 3300000, 3300000);
		regulator_enable(wifi_ldo);
	} else {
		usi_msg("regulator off\n");
		regulator_disable(wifi_ldo);
	}
	return 0;
}

#endif

static int usi_bm01a_gpio_ctrl(char* name, int level)
{
    struct mmc_pm_ops *ops = &mmc_card_pm_ops;
    char* gpio_cmd[6] = {"usi_bm01a_wl_regon", "usi_bm01a_bt_regon", "usi_bm01a_wl_rst", 
                               "usi_bm01a_wl_wake", "usi_bm01a_bt_rst", "usi_bm01a_bt_wake"};
    int i = 0;
    int ret = 0;
    
    for (i=0; i<6; i++) {
        if (strcmp(name, gpio_cmd[i])==0)
            break;
    }
    if (i==6) {
        usi_msg("No gpio %s for USI-BM01A module\n", name);
        return -1;
    }
    
//    usi_msg("Set GPIO %s to %d !\n", name, level);
    if (strcmp(name, "usi_bm01a_wl_regon") == 0) {
        if (level) {
            if (usi_bm01a_bt_on) {
                usi_msg("USI-BM01A is already powered up by bluetooth\n");
                goto change_state;
            } else {
                usi_msg("USI-BM01A is powered up by wifi\n");
                goto power_change;
            }
        } else {
            if (usi_bm01a_bt_on) {
                usi_msg("USI-BM01A should stay on because of bluetooth\n");
                goto change_state;
            } else {
                usi_msg("USI-BM01A is powered off by wifi\n");
                goto power_change;
            }
        }
    }
    
    if (strcmp(name, "usi_bm01a_bt_regon") == 0) {
        if (level) {
            if (usi_bm01a_wl_on) {
                usi_msg("USI-BM01A is already powered up by wifi\n");
                goto change_state;
            } else {
                usi_msg("USI-BM01A is powered up by bt\n");
                goto power_change;
            }
        } else {
            if (usi_bm01a_wl_on) {
                usi_msg("USI-BM01A should stay on because of wifi\n");
                goto change_state;
            } else {
                usi_msg("USI-BM01A is powered off by bt\n");
                goto power_change;
            }
        }
    }
    
    
    ret = gpio_write_one_pin_value(ops->pio_hdle, level, name);
    if (ret) {
        usi_msg("Failed to set gpio %s to %d !\n", name, level);
        return -1;
    }
    
    return 0;
    
power_change:
    #if CONFIG_CHIP_ID==1123
    ret = gpio_write_one_pin_value(ops->pio_hdle, level, "usi_bm01a_wl_pwr");
    #elif CONFIG_CHIP_ID==1125
    ret = usi_bm01a_power_onoff(level);
    #else
    #error "Found wrong chip id in wifi onoff\n"
    #endif
    if (ret) {
        usi_msg("Failed to power off USI-BM01A module!\n");
        return -1;
    }
    mdelay(5);
    ret = gpio_write_one_pin_value(ops->pio_hdle, level, "usi_bm01a_wlbt_regon");
    if (ret) {
        usi_msg("Failed to regon off for  USI-BM01A module!\n");
        return -1;
    }
    
change_state:
    if (strcmp(name, "usi_bm01a_wl_regon")==0)
        usi_bm01a_wl_on = level;
    if (strcmp(name, "usi_bm01a_bt_regon")==0)
        usi_bm01a_bt_on = level;
    usi_msg("USI-BM01A power state change: wifi %d, bt %d !!\n", usi_bm01a_wl_on, usi_bm01a_bt_on);
    return 0;
}

static int usi_bm01a_get_gpio_value(char* name)
{
    struct mmc_pm_ops *ops = &mmc_card_pm_ops;
    char* bt_hostwake =  "usi_bm01a_bt_hostwake";
    
    if (strcmp(name, bt_hostwake)) {
        usi_msg("No gpio %s for USI-BM01A\n", name);
        return -1;
    }
    
    return gpio_read_one_pin_value(ops->pio_hdle, name);
}

static void usi_bm01a_power(int mode, int* updown)
{
	if (mode) {
		if (*updown) {
            usi_bm01a_gpio_ctrl("usi_bm01a_wl_regon", 1);
            usi_bm01a_gpio_ctrl("usi_bm01a_wl_rst", 1);
		} else {
            usi_bm01a_gpio_ctrl("usi_bm01a_wl_rst", 0);
            usi_bm01a_gpio_ctrl("usi_bm01a_wl_regon", 0);
		}
	} else {
        if (usi_bm01a_wl_on)
            *updown = 1;
        else
            *updown = 0;
		usi_msg("sdio wifi power state: %s\n", usi_bm01a_wl_on ? "on" : "off");
	}
}

void usi_bm01a_gpio_init(void)
{
    struct mmc_pm_ops *ops = &mmc_card_pm_ops;
    usi_bm01a_wl_on = 0;
    usi_bm01a_bt_on = 0;
    ops->gpio_ctrl = usi_bm01a_gpio_ctrl;
    ops->get_io_val = usi_bm01a_get_gpio_value;
    ops->power = usi_bm01a_power;
}
