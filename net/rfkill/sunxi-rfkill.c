#include <linux/module.h>
#include <linux/init.h>
#include <linux/rfkill.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <mach/sys_config.h>

#if defined CONFIG_BT_HCIUART_DEBUG
#define RF_MSG(...)     do {printk("[rfkill]: "__VA_ARGS__);} while(0)
#else
#define RF_MSG(...)
#endif

#if (defined CONFIG_MMC_SUNXI_POWER_CONTROL)
extern int mmc_pm_get_mod_type(void);
extern int mmc_pm_gpio_ctrl(char* name, int level);
extern int mmc_pm_get_io_val(char* name);
#else
static __inline int mmc_pm_get_mod_type(void) {return 0;}
static __inline int mmc_pm_gpio_ctrl(char* name, int level) {return -1;}
static __inline int mmc_pm_get_io_val(char* name) {return -1;}
#endif

static DEFINE_SPINLOCK(bt_power_lock);
static const char bt_name[] = "bcm4329";
static struct rfkill *sw_rfkill;
static int rfkill_set_power(void *data, bool blocked)
{
    unsigned int mod_sel = mmc_pm_get_mod_type();
    
    RF_MSG("rfkill set power %d\n", blocked);
    
    spin_lock(&bt_power_lock);
    switch (mod_sel)
    {
        case 2: /* usi bm01a */
            if (!blocked) {
                mmc_pm_gpio_ctrl("usi_bm01a_bt_regon", 1);
                mmc_pm_gpio_ctrl("usi_bm01a_bt_rst", 1);
            } else {
                mmc_pm_gpio_ctrl("usi_bm01a_bt_regon", 0);
                mmc_pm_gpio_ctrl("usi_bm01a_bt_rst", 0);
            }
            break;
        case 5: /* swb b23 */
            if (!blocked) {
                mmc_pm_gpio_ctrl("swbb23_bt_shdn", 1);
            } else {
                mmc_pm_gpio_ctrl("swbb23_bt_shdn", 0);
            }
            break;
        case 6: /* huawei mw269x */
            if (!blocked) {
                mmc_pm_gpio_ctrl("hw_mw269x_bt_wake", 1);
                mmc_pm_gpio_ctrl("hw_mw269x_bt_enb", 1);
            } else {
                mmc_pm_gpio_ctrl("hw_mw269x_bt_enb", 0);
                mmc_pm_gpio_ctrl("hw_mw269x_bt_wake", 0);
            }
            break;
        case 8: /* bcm40183 */
            if (!blocked) {
                mmc_pm_gpio_ctrl("bcm40183_bt_regon", 1);
                mmc_pm_gpio_ctrl("bcm40183_bt_rst", 1);
            } else {
                mmc_pm_gpio_ctrl("bcm40183_bt_regon", 0);
                mmc_pm_gpio_ctrl("bcm40183_bt_rst", 0);
            }
            break;
         case 9: /* realtek rtl8723as */
            if (!blocked) {
                mmc_pm_gpio_ctrl("rtk_rtl8723as_bt_dis", 1);
            } else {
                mmc_pm_gpio_ctrl("rtk_rtl8723as_bt_dis", 0);
            }
            break;            
        default:
            RF_MSG("no bt module matched !!\n");
    }
    
    spin_unlock(&bt_power_lock);
    msleep(100);
    return 0;
}

static struct rfkill_ops sw_rfkill_ops = {
    .set_block = rfkill_set_power,
};

static int sw_rfkill_probe(struct platform_device *pdev)
{
    int ret = 0;

    sw_rfkill = rfkill_alloc(bt_name, &pdev->dev, 
                        RFKILL_TYPE_BLUETOOTH, &sw_rfkill_ops, NULL);
    if (unlikely(!sw_rfkill))
        return -ENOMEM;

    ret = rfkill_register(sw_rfkill);
    if (unlikely(ret)) {
        rfkill_destroy(sw_rfkill);
    }
    return ret;
}

static int sw_rfkill_remove(struct platform_device *pdev)
{
    if (likely(sw_rfkill)) {
        rfkill_unregister(sw_rfkill);
        rfkill_destroy(sw_rfkill);
    }
    return 0;
}

static struct platform_driver sw_rfkill_driver = {
    .probe = sw_rfkill_probe,
    .remove = sw_rfkill_remove,
    .driver = { 
        .name = "sunxi-rfkill",
        .owner = THIS_MODULE,
    },
};

static struct platform_device sw_rfkill_dev = {
    .name = "sunxi-rfkill",
};

static int __init sw_rfkill_init(void)
{
    platform_device_register(&sw_rfkill_dev);
    return platform_driver_register(&sw_rfkill_driver);
}

static void __exit sw_rfkill_exit(void)
{
    platform_device_unregister(&sw_rfkill_dev);
    platform_driver_unregister(&sw_rfkill_driver);
}

module_init(sw_rfkill_init);
module_exit(sw_rfkill_exit);

MODULE_DESCRIPTION("sunxi-rfkill driver");
MODULE_AUTHOR("Aaron.yemao<leafy.myeh@reuuimllatech.com>");
MODULE_LICENSE(GPL);

