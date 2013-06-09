#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#if 0
int main(int argc, char **argv)
{
	int 	fd;
	int		idx = 0;
	unsigned char	data[300*150];

	fd = open("/dev/fd0", O_WRONLY);

	for(idx=0;idx<300*150;idx++)
		data[idx] = (unsigned char)(idx%256);

	write(fd, (unsigned char*)data, 300*150);

	return 0;
}
#else
#include <linux/fb.h>

#define FBDEVFILE "/dev/fb0"

int main(int argc, char **argv)
{
	int fbfd;
	int ret;
	struct fb_var_screeninfo fbvar;
	struct fb_fix_screeninfo fbfix;

	fbfd = open(FBDEVFILE, O_RDWR);
	if(fbfd < 0)
	{
		printf("ERR : fbdev open\n");

		exit(1);
	}

	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);
	if(ret < 0)
	{
		printf("ERR : fbdev ioctl(FSCREENINFO)\n");
		exit(1);
	}

	ret = ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix);
	if(ret < 0)
	{
		printf("ERR : fbdev ioctl(FSCREENINFO)\n");
		exit(1);
	}

	printf("x-resolution : %d\n", fbvar.xres);
	printf("y-resolution : %d\n", fbvar.yres);
	printf("x-resolution(virtual) : %d\n", fbvar.xres_virtual);
	printf("y-resolution(virtual) : %d\n", fbvar.yres_virtual);
	printf("bpp : %d\n", fbvar.bits_per_pixel);

	printf("ERR : length of frame buffer memory : %d\n", fbfix.smem_len);
#if 1
{
	char data[640*480*16];
	int	idx = 0;
	int div = 1920;

	if(argc>1)
		div = atoi(argv[1]);

	for(idx=0;idx<640*480*16;idx++)
	{
		data[idx] = idx/div;
	}
	write(fbfd, data, sizeof(data));
}
#endif
	close(fbfd);
	exit(0);
	return 0;
}
#endif
