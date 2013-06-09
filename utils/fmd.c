#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

void usage(void)
{
	printf("md addr [read_count]\n");
	exit(EXIT_FAILURE);
}
void sig_handler(int sig)
{
	printf("%s:%d\n",__FUNCTION__,sig);
	return;
}
int main(int argc, char *argv[])
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
	unsigned int	uival=0;
	unsigned int		target=0;
	int			page_size,page_mask;
	int			idx,count=1,res;

	int			cmd=3;
	unsigned long arg	=7;

	int		size;
	char	buf[16];

	if((argc < 2) || (argc > 4))
		usage();

	res = sscanf(argv[1],"%x",&target);
	if(res != 1)
		usage();
	if(argc == 3)
	{
		res = sscanf(argv[2],"%x",&count);
		if(res != 1)
			usage();
		count /= 4;
	}
//	printf("%x:%x\n",target,count);

	page_size = sysconf(_SC_PAGE_SIZE);
	page_mask = page_size - 1;
//	printf("ps=%d,pm=%d\n",page_size,page_mask);
	length = page_size;
	if((fd = open("/dev/sfpga",O_RDWR|O_SYNC)) == -1)
	{
		printf("open error\n");
		return(1);
	}
//SIGNAL
{
struct sigaction	sa,osa;

	if(fcntl(fd,F_SETOWN,getpid()) == -1)
	{
		printf("fcntl error:F_SETOWN\n");
		return(1);
	}
	if(fcntl(fd,F_SETFL,fcntl(fd,F_GETFL) | O_ASYNC) == -1)
	{
		printf("fcntl error:F_SETFL\n");
		return(1);
	}
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGIO);
	sa.sa_flags = 0;
	sa.sa_handler = sig_handler;
	sigaction(SIGIO,&sa,&osa);
}

//	printf("open ok\n");
	if(ioctl(fd,cmd,&arg) < 0)
	{
		printf("ioctl error\n");
		exit(1);
	}
	if(ioctl(fd,cmd,&arg) < 0)
	{
		printf("ioctl error\n");
		exit(1);
	}
	size = read(fd,(char*)buf,1);
	printf("size=%d\n",size);
	map_base=mmap(addr,length,prot,flags,fd,offset);
	if(map_base == (void*)-1)
	{
		printf("mapping error\n");
		return(1);
	}
//	printf("map ok\n");
	virt_addr = map_base + (target & page_mask);

//	printf("mb=%p,va=%p,pa=%08x\n",map_base,virt_addr,virt_to_phys(map_base));
	printf("mb=%p,va=%p\n",map_base,virt_addr);
	for(idx = 0 ; idx < count ; ++idx)
	{
		if((idx%4) == 0)
			printf("\n%08x : ",target+idx*4);
		uival = *((unsigned int*)virt_addr+idx);
		printf("%08x ",uival);
	}
	printf("\n");

	close(fd);
	if(munmap(addr,length) == -1)
	{
		printf("unmapping error\n");
		return(1);
	}
//	printf("unmap ok\n");

	return(0);
}
