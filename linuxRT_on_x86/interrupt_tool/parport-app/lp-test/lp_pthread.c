#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/ioctl.h>
#include <linux/lp.h>
#include <fcntl.h>
#include <linux/ppdev.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <sys/time.h>
//#include <linux/time.h>
#include <linux/parport.h>
#define parportaddr 0x378
#define parportctr parportaddr+2

struct timeval tv1,tv2;


int status_pins(int fd) 
{
	int val; 
	ioctl(fd, PPRSTATUS, &val); 
	val^=PARPORT_STATUS_BUSY; /* /BUSY needs to get inverted */
//	printf("/BUSY = %s\n", ((val & PARPORT_STATUS_BUSY)==PARPORT_STATUS_BUSY)?"HI":"LO");
//	printf("ERROR = %s\n", ((val & PARPORT_STATUS_ERROR)==PARPORT_STATUS_ERROR)?"HI":"LO");
	printf("SELECT = %s\n", ((val & PARPORT_STATUS_SELECT)==PARPORT_STATUS_SELECT)?"HI":"LO");
//	printf("PAPEROUT = %s\n", ((val & PARPORT_STATUS_PAPEROUT)==PARPORT_STATUS_PAPEROUT)?"HI":"LO");
	printf("ACK = %s\n", ((val & PARPORT_STATUS_ACK)==PARPORT_STATUS_ACK)?"HI":"LO");
	return 0; 
}

int write_data(int fd, unsigned char data)
{
	return(ioctl(fd, PPWDATA, &data)); 
}
/*
int read_data(int fd)
{
	cdint mode, res;
	unsigned char data;

	mode = IEEE1284_MODE_ECP;
	res=ioctl(fd, PPSETMODE, &mode); /* ready to read ? */
/*	mode=255;
	res=ioctl(fd, PPDATADIR, &mode); /* switch output driver off */ 
/*	printf("ready to read data !\n");
	fflush(stdout);
	sleep(10);
	res=ioctl(fd, PPRDATA, &data); /* now fetch the data! */ 
/*	printf("data=%02x\n", data); 
	fflush(stdout);
	sleep(10); 
	data=0; 
	res=ioctl(fd, PPDATADIR, data); return 0; 
}
*/
int  main()
{
	int fd;
	int i,j;
	unsigned char buff;
	size_t count_write;
	pid_t pid;

	int result = ioperm(parportaddr,5,1);

	fd = open("/dev/parport0",O_RDWR);
	if(fd < 0)
	{
		perror("/dev/parport0:can not open");
		return 10;
	}
	
	if(ioctl(fd, PPCLAIM))
	{
		perror("PPCLAIM");
		close(fd);
		return 10;
	}

	outb(0x10,0x378+2);
	printf("wait interrupt!\n");
	
//	ioctl(fd,PPWCONTROL,0x55);
//	int val = read_data(fd);
//	printf("return read_data of val:%d\n",val);
//	status_pins(fd);
	
//	ioctl(fd,PPWCTLONIRQ,0x10);
	
	/*set interrupt event status to wait interrupt*/
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(fd,&rfds);
	
	printf("sleep 2s for creat interrupt signal!\n");
	sleep(2);
	/*creat interrupt event!*/
	if((pid = fork()) == 0)
	{
//		ioctl(fd,PPWDATA,0xff);
		write_data(fd,0xff);
//		getchar();
//		ioctl(fd,PPWDATA,0x00);
//		write_data(fd,0x00);
//		getchar();
		gettimeofday(&tv1,NULL);
		write_data(fd,0x00);
	}

	/*wait interrupt event!*/
	while(1)	
	{
		if(select(fd+1,&rfds,NULL,NULL,NULL))
		{
			gettimeofday(&tv2,NULL);
			break;
		}
	}

	printf("pid is:%d\n",getpid());
	status_pins(fd);
//	printf("i am out select!\n");
	printf("start:%9i.%06i\nfinsh:%9i.%06i\nLatency:%17i\n",(int)tv1.tv_sec,(int)tv1.tv_usec,(int)tv2.tv_sec,(int)tv2.tv_usec,(int)(tv2.tv_usec - tv1.tv_usec));
	
//	write_data(fd,0x00);
//	getchar();
//	ioctl(fd,PPWCONTROL,0x55);
	ioperm(parportaddr,5,0);
	ioctl(fd,PPRELEASE);
	close(fd);
	return 0;
}
