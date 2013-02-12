#ifndef	_PVDSPB_ADC_H_
#define	_PVDSPB_ADC_H_

#define	PVDSPB_ADC_MAJOR	0	/* Dynamic allocation */
#define	PVDSPB_ADC_NUM_DEVS	1	/* ADC1 only, at this moment */
#define SAMPLE_SIZE		2	/* 16 bit */
#define NUM_SAMPLES		256	/* 256 samples per set */
#define BUF_MAX_CNT		15	/* 16 sets per buffer */
#define BUF_MIN_CNT		0	/* Do not change this value */
#define BUF_SIZE		8192	/* Buffer size : 256*16*2 */

#define ADC1_INTRT		171

#define FPGA_BASE_ADDR		0x28000000
#define ADC_CTRL_OFFSET		0x6
#define ADC1_ADDR_OFFSET	0x400
#define ADC2_ADDR_OFFSET	0x600

#define ADC1_START		0x08
#define ADC2_START		0x80

#endif	/* _PVDSPB_ADC_H_ */
