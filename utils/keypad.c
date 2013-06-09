#include <stdio.h>
#include <stdlib.h>
//#include <io.h>
#include <fcntl.h>
//#include <alloc.h>
 #include <unistd.h>
 #include <linux/input.h>

int main(int argc, char **argv)
{
	int fd;
#if 0
	char buf[100];
	int i =0;
#endif
	int readnum = 0;
	struct input_event	input;

	fd = open("/dev/input/event0", O_RDONLY);
	if(fd<0)
	{
		printf("ERROR : open fail\n");
		exit(0);
	}

	do
	{
#if 1
	readnum = read(fd, (unsigned char*)&input, sizeof(struct input_event));
	printf("n=%d:t=%d:c=%0d:v=%d[%x]\n",readnum,input.type,input.code,input.value,input.value);
#else
	readnum = read(fd, buf, 100);
	
	for(i=0;i<readnum;i++)
	{
		if((i%16) == 0)
			printf("\n");
		printf("%02x ", buf[i]);
	}
	printf("\n");
#endif
	}while(1);



	return 0;
}
