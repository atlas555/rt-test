#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/ioctl.h>
#include <linux/lp.h>
#include <fcntl.h>
#include <linux/ppdev.h>
#include <linux/parport.h>

#define parportaddr 0x378+2

int main()
{
	int fd;
	int i,j;
	unsigned char buff;
	int mode;

	fd = open("/dev/lp0",O_RDWR);
	if(fd < 0)
	{
		perror("/dev/lp0:can not open");
		return 10;
	}
	
	ioctl(fd,PP_TRUST_IRQ);
	while(1)
	{
		if(read(fd,&buff,1) == -1)
			printf("blocking read fail!\n");
		write(fd,"1",1);
		for(j = 0;j < 10;j++)
		{
			write(fd,"3",1);
		}
		printf("%c  ",buff);
	}

	ioctl(fd,parportaddr,0x00);
	close(fd);
	return 0;
}
