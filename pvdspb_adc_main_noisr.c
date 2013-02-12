/*
 * Device driver for dspb_adc
 * Written by hl1oap
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* for printk() */
#include <linux/slab.h>		/* for kmalloc() */
#include <linux/errno.h>	/* for error codes */
#include <linux/types.h>	/* for size_t */
#include <linux/fs.h>		/* for register_chrdev_region() */
#include <linux/cdev.h>		/* for cdev structure */
#include <linux/interrupt.h>	/* for interrupt related features */
#include <linux/gpio.h>		/* for gpio_to_irq()	*/
#include <linux/string.h>	/* for memcpy()	*/
#include <asm/uaccess.h>	/* for copy_to_user()	*/

#include "pvdspb_adc.h"

int pvdspb_adc_major = PVDSPB_ADC_MAJOR;
int pvdspb_adc_minor = 0;
int pvdspb_adc_num_devs = PVDSPB_ADC_NUM_DEVS;	/* Num. of devices */

unsigned int adc1_cur_cnt, adc2_cur_cnt;
u16 *adc1_buf, *adc2_buf;

void __iomem *adc1_vir_addr;
void __iomem *adc2_vir_addr;

static int irq_adc1 = -1;
static int irq_adc2 = -1;

MODULE_AUTHOR("Ilkyoung Kwoun");
MODULE_LICENSE("Dual BSD/GPL");

static struct cdev *pvdspb_adc1_cdev;

irqreturn_t adc1_isr(int irq, void *dev_id) {

	memcpy(adc1_buf + (adc1_cur_cnt * NUM_SAMPLES), adc1_vir_addr, SAMPLE_SIZE * NUM_SAMPLES);
	if(adc1_cur_cnt == BUF_MAX_CNT) {
		adc1_cur_cnt = BUF_MIN_CNT;
	} else {
		adc1_cur_cnt++;
	}

	return IRQ_HANDLED;
}

int pvdspb_adc1_open(struct inode *inode, struct file *filp) {
	
	return nonseekable_open(inode, filp);
}

int pvdspb_adc1_release(struct inode *inode, struct file *filp) {
	return 0;
}

ssize_t pvdspb_adc1_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
	ssize_t retval = count;
	int err, adc1_prev_cnt;

/* hl1oap : temporally changed to read from FPGA directly.
	if(adc1_cur_cnt == BUF_MIN_CNT) {
		adc1_prev_cnt = BUF_MAX_CNT;
	} else {
		adc1_prev_cnt = adc1_cur_cnt - 1;
	}

	if((err = copy_to_user(buf, adc1_buf + (adc1_prev_cnt * NUM_SAMPLES), SAMPLE_SIZE * NUM_SAMPLES)) < 0) {
		retval = err;
	}
*/
	if((err = copy_to_user(buf, adc1_vir_addr, SAMPLE_SIZE * NUM_SAMPLES)) < 0) {
		retval = err;
	}

	return retval;
}

static const struct file_operations pvdspb_adc1_fops = {
	.owner =	THIS_MODULE,
	.open =		pvdspb_adc1_open,
	.release =	pvdspb_adc1_release,
	.read =		pvdspb_adc1_read,
};

void pvdspb_adc_cleanup_module(void) {
	dev_t devno;

	if(irq_adc1 > 0) {
		free_irq(irq_adc1, NULL);
	}

	devno = MKDEV(pvdspb_adc_major, pvdspb_adc_minor);

	if(adc1_buf) {
		kfree(adc1_buf);
	}
	if(adc2_buf) {
		kfree(adc2_buf);
	}

	if(adc1_vir_addr != NULL) {
		iounmap(adc1_vir_addr);
	}
	if(adc2_vir_addr != NULL) {
		iounmap(adc2_vir_addr);
	}

	unregister_chrdev_region(devno, pvdspb_adc_num_devs);

	printk(KERN_NOTICE "pvdspb_adc module removed!\n");
}

static int __init pvdspb_adc_init_module(void) {
	int result;
	dev_t dev = 0;

	/* Get the major and minor numbers */
	if(pvdspb_adc_major) {
		dev = MKDEV(pvdspb_adc_major, pvdspb_adc_minor);
		result = register_chrdev_region(dev, pvdspb_adc_num_devs, "pvdspb_adc");
	} else {	/* Dynamic allocation */
		result = alloc_chrdev_region(&dev, pvdspb_adc_minor, pvdspb_adc_num_devs, "pvdspb_adc");
		pvdspb_adc_major = MAJOR(dev);
	}
	if(result < 0) {
		printk(KERN_WARNING "pvdspb_adc: major %d allocation failed.\n", pvdspb_adc_major);
		goto fail;
	} 

	/* Device initialization */
	pvdspb_adc1_cdev = cdev_alloc();
	cdev_init(&pvdspb_adc1_cdev, &pvdspb_adc1_fops);
	cdev_add(&pvdspb_adc1_cdev, dev, 1);

	/* Buffer allocation */
	adc1_buf = kmalloc(BUF_SIZE, GFP_KERNEL);
	if(!adc1_buf) {
		result = -ENOMEM;
		goto fail;
	} else {
		memset(adc1_buf, 0, BUF_SIZE);
		adc1_cur_cnt = 0;
	}

	adc2_buf = kmalloc(BUF_SIZE, GFP_KERNEL);
	if(!adc2_buf) {
		result = -ENOMEM;
		goto fail;
	} else {
		memset(adc2_buf, 0, BUF_SIZE);
		adc2_cur_cnt = 0;
	}
	
	/* Setup GPIO 171 as interrupt source for ADC1 */
	gpio_request(ADC1_INTRT, NULL);		/* GPIO 171 */
	gpio_direction_input(ADC1_INTRT);
	irq_adc1 = gpio_to_irq(ADC1_INTRT);

	/* Get the virtual address for the adc1 and adc2 */
	adc1_vir_addr = ioremap_nocache(ADC1_ADDR, PAGE_SIZE);
	adc2_vir_addr = ioremap_nocache(ADC2_ADDR, PAGE_SIZE);
	if((adc1_vir_addr == NULL) || (adc2_vir_addr == NULL)) {
		printk(KERN_WARNING "pvdspb_adc: ADC mapping failed.\n");
		result = -ENOMEM;
		goto fail;
	}

	result = request_irq(irq_adc1, &adc1_isr, IRQF_TRIGGER_RISING, "pvdspb_adc1", NULL);
	/* hl1oap : Check and get the correct flag.
		    There are more in linux/interrupt.h
		#define IRQF_TRIGGER_NONE 0x00000000
		#define IRQF_TRIGGER_RISING 0x00000001
		#define IRQF_TRIGGER_FALLING 0x00000002
		#define IRQF_TRIGGER_HIGH 0x00000004
		#define IRQF_TRIGGER_LOW 0x00000008
	*/
/* hl1oap : temporally disable isr.
	if(result == 0) {
		printk(KERN_INFO "pvdspb_adc1 : can't get assigned irq%i\n", irq_adc1);
		irq_adc1 = -1;
		goto fail;
	} else {
		printk(KERN_NOTICE "pvdspb_adc module loaded!\n");
		return 0;	
	}
*/
	printk(KERN_NOTICE "pvdspb_adc module loaded!\n");
	return 0;	/* Initialization suceed */

  fail:		/* In case initialization fails */
	pvdspb_adc_cleanup_module();
	return result;
}

module_init(pvdspb_adc_init_module);
module_exit(pvdspb_adc_cleanup_module);
