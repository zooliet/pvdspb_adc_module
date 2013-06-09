#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define SIO_KMMAP_ENABLE	0
#define HAVE_DEBUG_PRINT  1
#include "mdebug.h"

#define MAX_SER_NO	7
typedef struct
{
	unsigned short	tx_buf;
	unsigned short	rx_buf;
	unsigned short	txing_count;
	unsigned short	rx_count;
	unsigned short	dummy[4];
}STserial;
typedef struct
{
	unsigned short	dummy_00[0x30];
	unsigned short	dummy_60[0x08];
	unsigned short	dummy_70;
	unsigned short	reset;
	unsigned short	tx_int;
	unsigned short	rx_int;
	unsigned short	dummy_78[0x04];
	STserial				serial[MAX_SER_NO];
}STfpga;

static STfpga	*pFPGA=NULL;
static int			memfd=-1;
static int			fpgafd=-1;

void sig_action(int sig,siginfo_t *info, void *ucontext)
{
int	idx;
unsigned short	tx_int,rx_int,rx_data,rx_count;

#if 0
	printf("%s:%d,i=%p,u=%p\n",__FUNCTION__,sig,info,ucontext);
	printf("signo=%d,errno=%d,code=%d\n",info->si_signo,info->si_errno,info->si_code);
	printf("band=%d,fd=%d\n",(int)info->si_band,info->si_fd);
#endif
	tx_int = pFPGA->tx_int;
	rx_int = pFPGA->rx_int;
	for(idx = 0 ; idx < MAX_SER_NO ; ++idx)
	{
		if((rx_int & (0x01 << idx)) == 0)
			continue;
		do
		{
			rx_data = pFPGA->serial[idx].rx_buf;
			pFPGA->serial[idx].tx_buf = rx_data;
			rx_count = pFPGA->serial[idx].rx_count & 0x1ff;
			DBG_PRT("idx=%d:%02x,%d\n",idx,rx_data,rx_count);
		}while(rx_count);
	}
	return;
}
void sig_handler(int sig)
{
	printf("%s:%d\n",__FUNCTION__,sig);
	return;
}
void exit_err(int val)
{
	if(memfd != -1)
		close(memfd);
	if(fpgafd != -1)
		close(fpgafd);
}
int main(int argc, char *argv[])
{
struct sigaction	sa,osa;
void		*addr =NULL;
size_t	length=0;
int			prot	=(PROT_READ | PROT_WRITE);
int			flags	=MAP_SHARED;
off_t		offset=0x24000000;

void		*map_base			=NULL;
int			page_size,page_mask;


	if((fpgafd = open("/dev/sfpga",O_RDWR|O_SYNC)) == -1)
	{
		printf("fpga open error\n");
		exit_err(1);
	}
	if(fcntl(fpgafd,F_SETOWN,getpid()) == -1)
	{
		printf("fcntl error:F_SETOWN\n");
		exit_err(1);
	}
	if(fcntl(fpgafd,F_SETFL,fcntl(fpgafd,F_GETFL) | O_ASYNC) == -1)
	{
		printf("fcntl error:F_SETFL\n");
		exit_err(1);
	}
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGIO);
#if 1
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = sig_action;
#else
	sa.sa_flags = 0;
	sa.sa_handler = sig_handler;
#endif
	sigaction(SIGIO,&sa,&osa);


	page_size = sysconf(_SC_PAGE_SIZE);
	page_mask = page_size - 1;
	length = page_size;
	if((memfd = open("/dev/mem",O_RDWR|O_SYNC)) == -1)
	{
		printf("mem open error\n");
		exit_err(1);
	}

	map_base=mmap(addr,length,prot,flags,memfd,offset);
	if(map_base == (void*)-1)
	{
		printf("mapping error\n");
		exit_err(1);
	}

	pFPGA=(STfpga*)map_base;
	pFPGA->reset = 0x01;

	do
	{
		sleep(1);
	}while(1);

	if(munmap(addr,length) == -1)
	{
		printf("unmapping error\n");
		exit_err(1);
	}
	exit_err(0);
	return(0);
}
