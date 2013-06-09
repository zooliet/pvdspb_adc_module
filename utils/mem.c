#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int mem_display(int argc, char *argv[],int bsize)
{
	void		*addr =NULL;
	size_t	length=0;
//	int			prot	=(PROT_READ | PROT_WRITE);
	int			prot	=(PROT_READ);
//	int			flags	=MAP_SHARED;
	int			flags	=MAP_PRIVATE;
	int			fd=-1;
	off_t		offset=0;

	void		*map_base			=NULL;
	void		*virt_addr		=NULL;
	unsigned char	ucval=0;
	unsigned short	usval=0;
	unsigned int	uival=0;
	unsigned int		target=0;
	int			page_size,page_mask;
	int			idx,count=1,res;

	if((argc < 2) || (argc > 3))
		return(1);
	res = sscanf(argv[1],"%x",&target);
	if(res != 1)
		return(1);
	if(argc == 3)
	{
		res = sscanf(argv[2],"%x",&count);
		if(res != 1)
			return(1);
		count /= bsize;
	}

	page_size = sysconf(_SC_PAGE_SIZE);
	page_mask = page_size - 1;
	length = page_size;
	if((fd = open("/dev/mem",O_RDWR|O_SYNC)) == -1)
	{
		printf("%d:open error\n",__LINE__);
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
	switch(bsize)
	{
		case 1:
			for(idx = 0 ; idx < count ; ++idx)
			{
				if((idx%16) == 0)
					printf("\n%08x : ",target+idx*bsize);
				if((idx%8) == 0)
					printf(" ");
				ucval = *((unsigned char*)virt_addr+idx);
				printf("%02x ",ucval);
			}
			printf("\n");
			break;
		case 2:
			for(idx = 0 ; idx < count ; ++idx)
			{
				if((idx%8) == 0)
					printf("\n%08x : ",target+idx*bsize);
				if((idx%4) == 0)
					printf(" ");
				usval = *((unsigned short*)virt_addr+idx);
				printf("%04x ",usval);
			}
			printf("\n");
			break;
		case 4:
			for(idx = 0 ; idx < count ; ++idx)
			{
				if((idx%4) == 0)
					printf("\n%08x : ",target+idx*bsize);
				if((idx%2) == 0)
					printf(" ");
				uival = *((unsigned int*)virt_addr+idx);
				printf("%08x ",uival);
			}
			printf("\n");
			break;
		default:
			return(1);
	}

	close(fd);
	if(munmap(addr,length) == -1)
	{
		printf("unmapping error\n");
		return(1);
	}
//	printf("unmap ok\n");

	return(0);
}
int mem_modify(int argc, char *argv[],int bsize)
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
		return(1);
	res = sscanf(argv[1],"%x",&target);
	if(res != 1)
		return(1);
	res = sscanf(argv[2],"%x",&uiVal);
	if(res != 1)
		return(1);

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
	switch(bsize)
	{
		case 1:
			*((unsigned char*)virt_addr) = uiVal;
			break;
		case 2:
			*((unsigned short*)virt_addr) = uiVal;
			break;
		case 4:
			*((unsigned int*)virt_addr) = uiVal;
			break;
		default:
			return(1);
	}

	close(fd);
	if(munmap(addr,length) == -1)
	{
		printf("unmapping error\n");
		return(1);
	}

	return(0);
}
