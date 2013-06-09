#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <string.h>
#include "bmp.h"

//#define DEFBMPFILE	"557733.bmp"
#define DEFBMPFILE	"1.bmp"
//#define DEFBMPFILE	"test.bmp"
//#define DEFBMPFILE	"bf1_240x320.bmp"
//#define DEFBMPFILE	"bf1_480x640.bmp"
#define FBDEVFILE		"/dev/fb0"
#define KEYDEVFILE	"/dev/input/event0"

static int						fbfd=-1;
static int						keyfd=-1;
static struct					fb_var_screeninfo fbvar;
static struct					fb_fix_screeninfo fbfix;
static unsigned char *fbbuf=NULL;
static int						fbsize=-1;
static int						pxsize=-1;

void exit_error(int val)
{
	if(fbfd != -1)
		close(fbfd);
	if(keyfd != -1)
		close(keyfd);
	if(fbbuf != NULL)
		munmap(fbbuf,fbsize);
	exit(val);
}

void ldisp_info(void)
{
	printf("Line length  : %d\n", fbfix.line_length);
	printf("x-resolution : %d\n", fbvar.xres);
	printf("y-resolution : %d\n", fbvar.yres);
	printf("x-resolution(virtual) : %d\n", fbvar.xres_virtual);
	printf("y-resolution(virtual) : %d\n", fbvar.yres_virtual);
	printf("bpp : %d\n", fbvar.bits_per_pixel);
	pxsize = (fbvar.bits_per_pixel/8);
	fbsize = fbvar.xres*fbvar.yres*pxsize;
	printf("fbsize = %d\n",fbsize);
	if((fbbuf = mmap(0,fbsize,PROT_READ|PROT_WRITE,MAP_SHARED,fbfd,0)) == (void*) -1)
	{
		printf("error mmap\n");
		exit_error(1);
	}
	printf("fbbuf = %p\n",fbbuf);
	return;
}

void ldisp_data(unsigned char *data)
{
int							idx,idy;

	for(idy = 0; idy < fbvar.yres ; ++idy)
	for(idx = 0; idx < fbvar.xres*pxsize ; ++idx)
		fbbuf[idy*fbvar.xres*pxsize+idx] = data[(fbvar.yres-idy)*fbvar.xres*pxsize+idx];
}

void ldisp_clear(void)
{
int	idx;

	if(pxsize == 4)
	{
		for(idx = 0; idx < fbsize ; idx+=pxsize)
		{
			if(idx < fbsize / 3)
			{
				fbbuf[idx+2] = 0xff;
				fbbuf[idx+1] = 0;
				fbbuf[idx+0] = 0;
			}
			else if(idx < (fbsize*2) / 3)
			{
				fbbuf[idx+2] = 0;
				fbbuf[idx+1] = 0xff;
				fbbuf[idx+0] = 0;
			}
			else
			{
				fbbuf[idx+2] = 0;
				fbbuf[idx+1] = 0;
				fbbuf[idx+0] = 0xff;
			}
		}
	}
	if(pxsize == 3)
	{
		for(idx = 0; idx < fbsize ; idx+=3)
		{
			if(idx < fbsize / 3)
			{
				fbbuf[idx+2] = 0xff;
				fbbuf[idx+1] = 0;
				fbbuf[idx+0] = 0;
			}
			else if(idx < (fbsize*2) / 3)
			{
				fbbuf[idx+2] = 0;
				fbbuf[idx+1] = 0xff;
				fbbuf[idx+0] = 0;
			}
			else
			{
				fbbuf[idx+2] = 0;
				fbbuf[idx+1] = 0;
				fbbuf[idx+0] = 0xff;
			}
		}
	}
}

int ldisp_bmp24_2_rgb565(char *filename, unsigned char *data)
{
int		bmfd;
int		readnum = 0;
int							bmp24_size;
unsigned char		*bmp24_data;
int							bmp_idx = 0,rgb_idx;

	bmfd = open(filename, O_RDONLY);
	if(bmfd < 0)
	{
		printf("ERROR : file open fail\n");
		printf("not found %s\n", filename);
		return 1;
	}

	read(bmfd, data, 54);	//file info
	bmp24_size = fbvar.xres*fbvar.yres*3;
	bmp24_data = malloc(bmp24_size+4);
	readnum = read(bmfd,bmp24_data, bmp24_size);
	close(bmfd);
	if(readnum != bmp24_size)
	{
		free(bmp24_data);
		return(1);
	}
#if 0
{
int	idx;
unsigned char		tmp;

	for(idx = 0 ; idx < bmp24_size ; idx+= 3)
	{
		tmp = bmp24_data[idx+0];
		bmp24_data[idx+0] = bmp24_data[idx+2];
		//bmp24_data[idx+1] = bmp24_data[idx+1];
		bmp24_data[idx+2] = tmp;

		if(idx == 0)
			printf("%d:%d:%d\n",bmp24_data[0],bmp24_data[1],bmp24_data[2]);
	}
}
#endif
	if(pxsize == 4)
	{
		for(rgb_idx = 0,bmp_idx=0 ; rgb_idx < fbsize ; rgb_idx+=pxsize,bmp_idx+=3)
		{
			data[rgb_idx+0] = bmp24_data[bmp_idx+0];
			data[rgb_idx+1] = bmp24_data[bmp_idx+1];
			data[rgb_idx+2] = bmp24_data[bmp_idx+2];
		}
	}
	else if(pxsize == 3)
	{
		memcpy(data,bmp24_data,fbsize);
	}
	else if(pxsize == 2)
	{
	unsigned short	*usdata=(unsigned short*)data;
		for(rgb_idx=0,bmp_idx = 0 ; rgb_idx < fbsize/2 ; bmp_idx+=3)
		{
			usdata[rgb_idx++] = CONV_24TO16(*(unsigned int*)&bmp24_data[bmp_idx]);
		}
		printf("%04x:e=%d\n",usdata[0],rgb_idx);
	}
	else
		printf("Can't Convert\n");
	free(bmp24_data);
	return 0;
}

void ldisp_file(char *fname)
{
unsigned char data[640*480*8];
int ret;

	ret=ldisp_bmp24_2_rgb565(fname,data);
	if(ret)
	{
		printf("converison error\n");
		exit_error(1);
	}
	ldisp_data(data);
}

void ldisp_direct(int key)
{
unsigned char data[640*480*8];
int						idx;

	if(pxsize == 4)
	{
	switch(key)
	{
		case 0x41 :
			for(idx = 0 ; idx < fbsize ; idx+= pxsize)
			{
				data[idx+2] = 0xff;
				data[idx+1] = 0;
				data[idx+0] = 0;
			}
			break;
		case 0x42 :
			for(idx = 0 ; idx < fbsize ; idx+= pxsize)
			{
				data[idx+2] = 0;
				data[idx+1] = 0xff;
				data[idx+0] = 0;
			}
			break;
		case 0x43 :
			for(idx = 0 ; idx < fbsize ; idx+= pxsize)
			{
				data[idx+2] = 0;
				data[idx+1] = 0;
				data[idx+0] = 0xff;
			}
			break;
		default :
			break;
	}
	}
	if(pxsize == 3)
	{
	switch(key)
	{
		case 0x41 :
			for(idx = 0 ; idx < fbsize ; idx+= 3)
			{
				data[idx+2] = 0xff;
				data[idx+1] = 0;
				data[idx+0] = 0;
			}
			break;
		case 0x42 :
			for(idx = 0 ; idx < fbsize ; idx+= 3)
			{
				data[idx+2] = 0;
				data[idx+1] = 0xff;
				data[idx+0] = 0;
			}
			break;
		case 0x43 :
			for(idx = 0 ; idx < fbsize ; idx+= 3)
			{
				data[idx+2] = 0;
				data[idx+1] = 0;
				data[idx+0] = 0xff;
			}
			break;
		default :
			break;
	}
	}
	if(pxsize == 2)
	{
	switch(key)
	{
		case 0x41 :
			for(idx = 0 ; idx < fbsize ; idx+= pxsize)
			{
				data[idx+1] = 0xf8;
				data[idx+0] = 0;
			}
			break;
		case 0x42 :
			for(idx = 0 ; idx < fbsize ; idx+= pxsize)
			{
				data[idx+1] = 0x07;
				data[idx+0] = 0xe0;
			}
			break;
		case 0x43 :
			for(idx = 0 ; idx < fbsize ; idx+= pxsize)
			{
				data[idx+1] = 0;
				data[idx+0] = 0x1f;
			}
			break;
		default :
			break;
	}
	}
	ldisp_data(data);
}

int main(int argc, char **argv)
{
struct input_event	input;
int	ret;
int			readnum;

	fbfd = open(FBDEVFILE, O_RDWR);
	if(fbfd < 0)
	{
		printf("ERR : fbdev open\n");
		exit_error(1);
	}

	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);
	if(ret < 0)
	{
		printf("ERR : fbdev ioctl(FSCREENINFO)\n");
		exit_error(1);
	}

	ret = ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix);
	if(ret < 0)
	{
		printf("ERR : fbdev ioctl(FSCREENINFO)\n");
		exit_error(1);
	}

	ldisp_info();

//	ldisp_file(DEFBMPFILE);

	keyfd = open(KEYDEVFILE,O_RDONLY);
	if(keyfd<0)
	{
		printf("ERROR : open fail\n");
		exit_error(0);
	}

	do
	{
		readnum = read(keyfd, (unsigned char*)&input, sizeof(struct input_event));
		if(readnum != sizeof(struct input_event))
		{
			printf("readnum=%d\n",readnum);
			exit_error(1);
		}
		if(input.type != 1)
			continue;
		if(input.value != 1)
			continue;
		printf("n=%d:t=%d:c=%x[%0d]:v=%d[%x]\n",readnum,input.type,input.code,input.code,input.value,input.value);
		switch(input.code)
		{
			case 0x11 :	ldisp_file("/usr/local/bin/1.bmp");	break;
			case 0x12 :	ldisp_file("/usr/local/bin/2.bmp");	break;
			case 0x13 :	ldisp_file("/usr/local/bin/3.bmp");	break;
			case 0x14	:	ldisp_file("/usr/local/bin/4.bmp");	break;
			case 0x21 :	ldisp_file("/usr/local/bin/1.bmp");	break;
			case 0x22 :	ldisp_file("/usr/local/bin/2.bmp");	break;
			case 0x23 :	ldisp_file("/usr/local/bin/3.bmp");	break;
			case 0x24	:	ldisp_file("/usr/local/bin/4.bmp");	break;
			case 0x31 :	ldisp_file("/usr/local/bin/1.bmp");	break;
			case 0x32 :	ldisp_file("/usr/local/bin/2.bmp");	break;
			case 0x33 :	ldisp_file("/usr/local/bin/3.bmp");	break;
			case 0x34	:	ldisp_file("/usr/local/bin/4.bmp");	break;
			case 0x41 :	ldisp_direct(input.code);	break;
			case 0x42 :	ldisp_direct(input.code);	break;
			case 0x43 :	ldisp_direct(input.code);	break;

			case 105 :	ldisp_direct(0x41);	break;
			case 106 :	ldisp_direct(0x42);	break;
			case 30 :	ldisp_direct(0x43);	break;
			case 0x30	:	ldisp_file("bf1_480x640.bmp");	break;

			case 0x44	:
			default :
				ldisp_clear();
				printf("EXIT program\n");
				exit_error(0);
				break;
		}
	}while(1);
	exit_error(0);
	return 0;
}
