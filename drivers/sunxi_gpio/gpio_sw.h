/*
 * 2011-2012
 * panlong <panlong@reuuimllatech.com>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef GPIO_SW_H_INCLUDED
#define GPIO_SW_H_INCLUDED
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/rwsem.h>
#include <linux/timer.h>
#define GPIO_TEST_BASE 0xf1c20800
#define GPIO_INT_BASE 0xf1c20400

struct gpio_sw_platdata {

	unsigned int		flags;
	char				name[16];

};
struct gpio_sw_classdev{
	const char	*name;
	u32	pio_hdle;
	int	port;					/*GPIOʹ�õĶ˿ں� 1:PA 2:PB .... */
	int port_num;				/*GPIO�ڵ�ǰ�˿ڵ����(�ڼ�������)  */
	int mul_sel;				/*GPIO�Ĺ���ѡ�� 0������  1����� */
	int pull;					/*GPIO�����õ���״̬ 0������裬1����������2�������� */
	int drv_level;				/*GPIO���������� ��0��3�ĸ��ȼ� */
	int data;					/*GPIO�ĵ�ƽ */
	int flags;
	char irq;
	char irq_num;
	u8 irq_type;

	#define SW_GPIO_TRIGER_POSITIVE		0x0
	#define SW_GPIO_TRIGER_NEGATIVE		0x1
	#define SW_GPIO_TRIGER_HIGH			0x2
	#define SW_GPIO_TRIGER_LOW			0x3
	#define SW_GPIO_TRIGER_DOUBLE		0x4

	#define SW_GPIO_SUSPENDED		(1 << 0)
	#define SW_GPIO_CORE_SUSPENDED		(1 << 16)
	int		(*gpio_sw_cfg_set)(struct gpio_sw_classdev *gpio_sw_cdev,
					int  mul_sel);		/*����gpio���������״̬ */
	int		(*gpio_sw_pull_set)(struct gpio_sw_classdev *gpio_sw_cdev,
					int  pull);			/*����gpio�ĵ������������Ǹ���������� */
	int		(*gpio_sw_data_set)(struct gpio_sw_classdev *gpio_sw_cdev,
					int  data);			/*����gpio�������ƽ*/
	int		(*gpio_sw_drv_level_set)(struct gpio_sw_classdev *gpio_sw_cdev,
					int  drv_level);	/*����gpio�������ȼ� */
	int		(*gpio_sw_cfg_get)(struct gpio_sw_classdev *gpio_sw_cdev);
										/*��ȡgpio��������� */
	int		(*gpio_sw_pull_get)(struct gpio_sw_classdev *gpio_sw_cdev);
										/*��ȡgpio�ĵ������������Ǹ���������� */
	int		(*gpio_sw_data_get)(struct gpio_sw_classdev *gpio_sw_cdev);
										/*��ȡpio�������ƽ*/
	int		(*gpio_sw_drv_level_get)(struct gpio_sw_classdev *gpio_sw_cdev);
										/*��ȡgpio�������ȼ� */
	struct device		*dev;
	struct list_head	 node;

};

extern void gpio_sw_classdev_suspend(struct gpio_sw_classdev *gpio_sw_cdev);
extern void gpio_sw_classdev_resume(struct gpio_sw_classdev *gpio_sw_cdev);

extern int gpio_sw_classdev_register(struct device *parent,
				struct gpio_sw_classdev *gpio_sw_cdev);
extern void gpio_sw_classdev_unregister(struct gpio_sw_classdev *gpio_sw_cdev);


#endif
