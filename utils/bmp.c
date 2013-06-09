#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "bmp.h"

#define FBDEVFILE "/dev/fb0"

int get_bmp_file(char *filename, unsigned char *data, int *size)
{
	int		bmfd, fbfd;
	int		readnum = 0;
	int		idx = 0;
	unsigned short m_Red=0;
	unsigned short m_Green=0;
	unsigned short m_Blue=0;

	bmfd = open(filename, O_RDONLY);
	if(bmfd < 0)
	{
		printf("ERROR : file open fail\n");
		printf("not found %s\n", filename);
		exit(0);
	}
	fbfd = open(FBDEVFILE, O_RDWR);
	if(fbfd < 0)
	{
		printf("ERR : fbdev open\n");
		exit(1);
	}

	read(bmfd, data, 54);
	for(idx=0;idx<640*960;idx++)
	{
#if 0
		if(idx < 54)
		{
			if((idx%4)==0)
				printf("\n");
			printf("0x%02x ", data[idx]);
		}
#endif
		data[idx] = 0xFF;
	}
	//	printf("\n");
	idx = 0;
	//	write(fbfd, data, 640*480*2);
	while(1)
	{
		unsigned char	tmp[4];
		int				*rgb24 = (int *)tmp;
		unsigned short	rgb16;

		readnum += read(bmfd, tmp, 3);
		if(readnum < 3)
		{
			printf("read error\n");
			break;
		}
		m_Red =  GET_RED(*rgb24);
		m_Green = GET_GREEN(*rgb24);
		m_Blue = GET_BLUE(*rgb24);

		rgb16 = 0;
		rgb16 += (m_Red&0xF8) << 8;
		rgb16 += (m_Green&0xFC) << 3;
		rgb16 += (m_Blue&0xF8) >> 3;
#if 0
		data[idx] = tmp[0];
		data[idx+1] = tmp[1];
		data[idx+2] = tmp[2];
		idx += 3;
#else
		data[idx] = rgb16&0xFF;
		data[idx+1] = (rgb16>>8)&0xFF;
		idx += 2;
#endif

		if(readnum >= 640*480*3)
			break;
	}

#if 0
unsigned short *pfbdata;
	pfbdata = (unsigned short *)mmap(0, fbvar.xres * fbvar.yres * 2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);  

	if((unsigned)pfbdata == (unsigned)-1)  
	{  
		perror("... .. ..!!");  
		exit(1);  
	}      

	temp = pfbdata;  

	for (y = 0; y < fbvar.yres; y++) {  
		for (x = 0; x < fbvar.xres; x++) {  
			*temp++ = 0xffff;  
		}  
	}  

	munmap(pfbdata, fbvar.xres * fbvar.yres * 2);  
#else
	for(idx=639;idx >= 0;idx--)
		write(fbfd, data+idx*960, 960);
#endif
	return 0;
}

int main(int argc, char **argv)
{
	unsigned char data[640*480*4];
	int	size = 0;

	if(argc < 2)
	{
		printf("ERROR : need more parameters\n");
		printf("usage : %s filename\n", argv[0]);
		exit(1);
	}

	get_bmp_file(argv[1], data, &size);

	return 0;
}
