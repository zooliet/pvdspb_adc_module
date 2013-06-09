#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HAVE_DEBUG_PRINT  1
#include "mdebug.h"
#include "mem.c"

void usage(void)
{
	printf("mm1 addr data\n");
	exit(EXIT_FAILURE);
}
int main(int argc, char *argv[])
{
	if(mem_modify(argc,argv,1))
		usage();
	return(0);
}
