#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
int main(int argc, char *argv[])
{
FILE	*fp;
unsigned char	val;
int			ret;
int             fd;
struct termios  vTERM;

	fd = open("/dev/ttyO2",O_RDONLY);
	if(fd)
		printf("fd=%d\n\r",fd);
	ret=tcgetattr(fd,&vTERM);
	if(ret)
		printf("ret=%d\n\r",ret);

	printf("%o:%o:%o:%o\n",vTERM.c_cflag,vTERM.c_cflag&CSTOPB,vTERM.c_cflag&CSIZE,vTERM.c_cflag&CBAUD);
#if 0
	vTERM.c_cflag &= ~CREAD;
#endif

	vTERM.c_cflag &= ~CBAUD;
	printf("%o:%o:%o:%o\n",vTERM.c_cflag,vTERM.c_cflag&CSTOPB,vTERM.c_cflag&CSIZE,vTERM.c_cflag&CBAUD);
#if 1
	vTERM.c_cflag |= B115200;
#else
	vTERM.c_cflag |= B38400;
	vTERM.c_cflag |= B9600;
#endif
	printf("%o:%o:%o:%o\n",vTERM.c_cflag,vTERM.c_cflag&CSTOPB,vTERM.c_cflag&CSIZE,vTERM.c_cflag&CBAUD);
	ret=tcsetattr(fd,TCSAFLUSH,&vTERM);
	if(ret)
		printf("ret=%d\n\r",ret);
	close(fd);

	fp = fopen("/dev/ttyO2", "w+");
	fprintf(fp,"ABCDEFG\n\r");
	do
	{
		fscanf(fp,"%c",&val);
		printf("0x%02x\n\r",val);
	}while(1);
	fclose(fp);
	return(0);
}

