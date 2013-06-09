#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define HAVE_DEBUG_PRINT	1
#include "mdebug.h"

void usage(void)
{
	printf("fmd addr [read_count]\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
int		fd=-1;
int		size;
char	buf[16];

	if((argc < 1) || (argc > 1))
		usage();

	if((fd = open("/dev/ttyG0",O_RDWR)) == -1)
	{
		printf("open error\n");
		return(1);
	}
	DBG_PRT("open ok\n");

	size = read(fd,(char*)buf,1);
	DBG_PRT("size=%d\n",size);

	close(fd);

	return(0);
}
