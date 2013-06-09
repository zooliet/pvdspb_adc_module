#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HAVE_DEBUG_PRINT  1
#include "mdebug.h"

void usage(void)
{
	printf("mm addr data\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	void		*addr =NULL;
	size_t	length=0;
	int			prot	=(PROT_READ | PROT_WRITE);
	int			flags	=MAP_SHARED;
	int			fd=-1;
	off_t		offset=0;

	void		*map_base			=NULL;
	void		*virt_addr		=NULL;
	unsigned int		target=0;
	unsigned int		uiVal=0;
	int			page_size,page_mask;
	int			res;

	if((argc < 3) || (argc > 3))
		usage();
	res = sscanf(argv[1],"%x",&target);
	if(res != 1)
		usage();
	res = sscanf(argv[2],"%x",&uiVal);
	if(res != 1)
		usage();

	page_size = sysconf(_SC_PAGE_SIZE);
	page_mask = page_size - 1;
	length = page_size;
	if((fd = open("/dev/mem",O_RDWR|O_SYNC)) == -1)
	{
		printf("open error\n");
		return(1);
	}

	offset = target & ~page_mask;
	map_base=mmap(addr,length,prot,flags,fd,offset);
	if(map_base == (void*)-1)
	{
		printf("mapping error\n");
		return(1);
	}

	virt_addr = map_base + (target & page_mask);
	*((unsigned int*)virt_addr) = uiVal;

	close(fd);
	if(munmap(addr,length) == -1)
	{
		printf("unmapping error\n");
		return(1);
	}

	return(0);
}
