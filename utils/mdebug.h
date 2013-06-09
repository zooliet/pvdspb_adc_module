#ifndef __MDEBUG_H__
#define __MDEBUG_H__
/******************************************************************************/
/******************************************************************************/
#if HAVE_DEBUG_PRINT

#ifdef __KERNEL__
#define DBG_FFL_PRT(fmt, args...) \
	printk("[%s:%s:%d] " fmt, __FILE__, __FUNCTION__, __LINE__, ## args)
#define DBG_FL_PRT(fmt, args...) \
	printk("[%s:%d] " fmt,__FUNCTION__, __LINE__, ## args)
#define DBG_PRT(fmt, args...) \
	printk("[%s:%d] " fmt,__FUNCTION__, __LINE__, ## args)
#define DBG_A_PRT(fmt, args...) \
	printf("[%s:%d] " fmt,__FUNCTION__, __LINE__, ## args)
#define DBG_E_PRT(fmt, args...) \
	printk("[ERR:%12s:%d] " fmt,__FUNCTION__, __LINE__, ## args)
#else
#define DBG_FFL_PRT(fmt, args...) \
	printf("[%s:%s:%d] " fmt, __FILE__, __FUNCTION__, __LINE__, ## args)
#define DBG_FL_PRT(fmt, args...) \
	printf("[%s:%d] " fmt,__FUNCTION__, __LINE__, ## args)
#define DBG_PRT(fmt, args...) \
	printf("[%16s:%d] " fmt,__FUNCTION__, __LINE__, ## args)
#define DBG_E_PRT(fmt, args...) \
	printf("[ERR:%12s:%d] " fmt,__FUNCTION__, __LINE__, ## args)
#define DBG_A_PRT(fmt, args...) \
	printf("[%s:%d] " fmt,__FUNCTION__, __LINE__, ## args)
#endif

#else

#define DBG_FFL_PRT(fmt, args...)
#define DBG_FL_PRT(fmt, args...)
#define DBG_PRT(fmt, args...)
#define DBG_A_PRT(fmt, args...)

#endif
/******************************************************************************/
/******************************************************************************/
#endif //__MDEBUG_H__
