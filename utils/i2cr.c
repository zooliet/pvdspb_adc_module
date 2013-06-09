#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define SLAVE_ADDR1	0x48
#define MAX_DATA_BUF_SIZE	1024

struct i2c_rdwr_ioctl_data	arg;
static struct i2c_msg xfer_msg[2];
static unsigned char		reg_addr_buf[1];
static unsigned char		data_buf[MAX_DATA_BUF_SIZE];

void usage(void)
{
	printf("i2cr bus_id dev_id reg_addr [count]\n");
	exit(EXIT_FAILURE);
}

void _make_read_arg(unsigned short dev_addr,unsigned char reg_addr,unsigned short data_len)
{
struct i2c_msg *msg;

	msg = &xfer_msg[0];
	msg->addr = dev_addr;
	msg->flags = 0;
	msg->len = 1;
	msg->buf = &reg_addr_buf[0];
	reg_addr_buf[0] = reg_addr;

	msg = &xfer_msg[1];
	msg->addr = dev_addr;
	msg->flags = I2C_M_RD;
	msg->len = data_len;
	msg->buf = &data_buf[0];

	arg.msgs = &xfer_msg[0];
	arg.nmsgs = 2;
}
void _make_write_arg(unsigned short dev_addr,unsigned char reg_addr,unsigned short data_len)
{
struct i2c_msg *msg;

	msg = &xfer_msg[0];
	msg->addr = dev_addr;
	msg->flags = 0;
	msg->len = data_len + 1;
	msg->buf = &data_buf[0];
	data_buf[0] = reg_addr;

	arg.msgs = &xfer_msg[0];
	arg.nmsgs = 1;
}

int main(int argc,char *argv[])
{
int	fd=-1;
int	idx;

unsigned int		bus_id;
unsigned int		dev_id = 0x48;
unsigned int		reg_addr = 0x0;
unsigned int		data_len = 0x1;
char						dev_name[16];
char						tmp[4];
int							res;

	if((argc < 4) || (argc > 5))
		usage();

	res = sscanf(argv[1],"%d",&bus_id);
	if(res != 1)
		usage();
	res = sscanf(argv[2],"%x",&dev_id);
	if(res != 1)
		usage();
	res = sscanf(argv[3],"%x",&reg_addr);
	if(res != 1)
		usage();
	if(argc == 5)
	{
		res = sscanf(argv[4],"%x",&data_len);
		if(res != 1)
			usage();
	}

	strcpy(dev_name,"/dev/i2c-");
	sprintf(tmp,"%d",bus_id);
	strcat(dev_name,tmp);
	printf("%s\n",dev_name);
	if((fd = open(dev_name,O_RDWR)) < 0)
	{
		printf("open error\n");
		exit(1);
	}
	printf("open ok\n");

	_make_read_arg(dev_id,reg_addr,data_len);
	if(ioctl(fd,I2C_RDWR,&arg) < 0)
	{
		printf("ioctl error\n");
		exit(1);
	}
	printf("ioctl ok\n");

	for(idx = 0; idx < data_len ; ++idx)
	{
		if((idx%16) == 0)
			printf("\n");
		printf("%02x ",data_buf[idx]);
	}
	printf("\n");
	exit(0);
}
