#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define SLAVE_ADDR1    					0x48
#define MAX_DATA_BUF_SIZE       1024

#define HAVE_DEBUG_PRINT	0
#include "mdebug.h"

void usage(void)
{
	printf("gps tty <wr|rd> [data]\n");
	printf("gps i2c\n");
	exit(EXIT_FAILURE);
}

void exit_error(char *str)
{
	if(str != NULL)
	{
		printf("%s",str);
		usage();
	}
}

int read_data(int fd, unsigned short dev_addr,unsigned char reg_addr, unsigned char *data, unsigned short data_len)
{
	struct i2c_rdwr_ioctl_data	arg;
	struct i2c_msg							xfer_msg[2];
	struct i2c_msg							*msg;
	
	msg					= &(xfer_msg[0]);
	msg->addr		= dev_addr;
	msg->flags	= 0;
	msg->len		= 1;
	msg->buf		= &reg_addr;

	msg					= &(xfer_msg[1]);
	msg->addr		= dev_addr;
	msg->flags	= I2C_M_RD;
	msg->len		= data_len;
	msg->buf		= data;


	arg.msgs		= &(xfer_msg[0]);
	arg.nmsgs		= 2;

	if(ioctl(fd, I2C_RDWR, &arg) < 0)
	{
		printf("IOCTRL : ERROR\n");
		return -1;
	}

	return 0;
}

int write_data(int fd, unsigned short dev_addr,unsigned char reg_addr, unsigned char *data, unsigned short data_len)
{
	int													i = 0;
	struct i2c_rdwr_ioctl_data	arg;
	struct i2c_msg							xfer_msg;
	struct i2c_msg							*msg;
	unsigned char								*buff;

	buff				= malloc(data_len + 1);
	msg					= &xfer_msg;
	msg->addr		= dev_addr;
	msg->flags	= 0;
	msg->len		= data_len + 1;
	msg->buf		= buff;

	buff[0]			= reg_addr;
	for(i=0;i<data_len;i++)
		buff[i+1] = data[i];

	arg.msgs		= (struct i2c_msg *)&(xfer_msg);
	arg.nmsgs		= 1;

	if(ioctl(fd, I2C_RDWR, &arg) < 0)
	{
		free(buff);
		printf("IOCTRL : ERROR\n");
		return -1;
	}

	free(buff);

	return 0;
}

int _tty_init()
{
	int fd							= -1;
	char	dev_name[16]	= {0,};

	strcpy(dev_name, "/dev/ttyG0");
	if((fd = open(dev_name, O_RDWR))<0)
		printf("open error\n");
	else
		printf("open ok\n");

	return fd;
}

int _i2c_initi(int bus_id)
{
	int		fd						= -1;
	char	dev_name[16]	= {0,};
	char	tmp[4]				= {0,};
	
	strcpy(dev_name,"/dev/i2c-");
	sprintf(tmp,"%d",bus_id);
	strcat(dev_name,tmp);
	printf("%s\n",dev_name);
	if((fd = open(dev_name,O_RDWR)) < 0)
	{
		printf("open error\n");
		return fd;
	}
	printf("open ok\n");
	return fd;
}

int main(int argc, char *argv[])
{
	int						fd				= -1;
	int						idx				= 0;
	int						res				= 0;
	unsigned int	bus_id		= 0;
	unsigned int	dev_id		= 0;
	unsigned int	data_len	= 0;
	unsigned char	data[MAX_DATA_BUF_SIZE] = {0,};
	unsigned char	t_data[MAX_DATA_BUF_SIZE];
	int						is_tty		= 0;

	if(argc > 1)
	{
		if(strcmp(argv[1], "tty")==0)
		{
			fd = _tty_init();
			is_tty = 1;	
		}
	}
	else
	{
		bus_id	= 2;
		dev_id	= 0x42;
		fd = _i2c_initi(bus_id);
		if(fd < 0)
			exit_error("Invalid bus_id or dev_id\n");
	}

	while(1)
	{
		if(is_tty)
		{
			if(strcmp(argv[2], "rd")==0)
			{
				printf("[%s:%d]read data start\n", __FUNCTION__, __LINE__);
				res = read(fd, data, sizeof(data));
				printf("[%s:%d]read data end\n", __FUNCTION__, __LINE__);
				if(res <= 0)
					break;
			}
			else if(strcmp(argv[2], "wr")==0)
			{
				sprintf((char *)data, "%s", argv[3]);
				printf("[%s:%d]write data [%s]\n", __FUNCTION__, __LINE__, data);
				write(fd, data, strlen((char *)data));
				printf("[%s:%d]write end\n", __FUNCTION__, __LINE__);
				break;
			}
			printf("[%s:%d]write end\n", __FUNCTION__, __LINE__);
		}
		else
		{
			res = read_data(fd, dev_id, 0xFF, data, 1);
			if(res < 0)
				break;
		}

		DBG_PRT("[%s:%d]write end\n", __FUNCTION__, __LINE__);
		if(data[0] == 0xFF)
		{
			if(data_len)
				printf("\n");

			data_len	= 0;
			idx				= 0;
			memset(t_data, 0x00, 16);
			continue;
		}
		t_data[idx++] = data[0];

		if((data[0] == 0x0D) || (data[0] == 0x0A))
			t_data[idx-1] = 0x2E;

		printf("0x%02X ", data[0]);
		if(idx==16)
		{
			printf("  : %s\n", t_data);
			idx = 0;
			memset(t_data, 0x00, 16);
		}

		if(data[0]==0x0A)
		{
			int i=0;
			for(i=16;i>idx;i--)
				printf("     ");

			printf("  : %s\n\n", t_data);
			idx = 0;
			memset(t_data, 0x00, 16);
		}

		data_len++;
	}

	printf("[%s:%d]write end\n", __FUNCTION__, __LINE__);
	close(fd);
	printf("\n program end\n");
	return 0;
}
