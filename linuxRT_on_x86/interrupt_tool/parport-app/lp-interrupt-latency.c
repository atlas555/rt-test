/*
 * lp-interrupt-latency.c
 *
 *   This is tool for measuring interrupt response latency with hardware  * parport.
 *
 *  Copyright (C) 2010 Zhang Xiaolong <wdzxl198@163.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation either version 2 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307, USA.
 */



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
#include <linux/parport.h>

#define parportaddr 0x378
#define parportctr parportaddr+2

struct timeval tv1,tv2;


int status_pins(int fd) 
{
	int val; 
	ioctl(fd, PPRSTATUS, &val); 
	val^=PARPORT_STATUS_BUSY; /* /BUSYeeds to get inverted */
	printf("SELECT = %s\n", ((val & PARPORT_STATUS_SELECT)==PARPORT_STATUS_SELECT)?"HI":"LO");
	printf("ACK = %s\n", ((val & PARPORT_STATUS_ACK)==PARPORT_STATUS_ACK)?"HI":"LO");
	return 0; 
}

int write_data(int fd, unsigned char data)
{
	return(ioctl(fd, PPWDATA, &data)); 
}

int  main()
{
	int fd;
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
	/*enable interrupt!*/
	outb(0x10,0x378+2);
//	printf("wait interrupt!\n");
	
    /*set interrupt event status to wait interrupt*/
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(fd,&rfds);
	
	/*creat interrupt event!*/
	if((pid = fork()) == 0)
	{
		write_data(fd,0xff);
		write_data(fd,0x00);
		gettimeofday(&tv1,NULL);
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
	
//	printf("pid is:%d\n",getpid());
//	printf("i am out select!\n");
//	printf("start:%9i.%06i\nfinsh:%9i.%06i\nLatency:%17i\n",(int)tv1.tv_sec,(int)tv1.tv_usec,(int)tv2.tv_sec,(int)tv2.tv_usec,(int)(tv2.tv_usec - tv1.tv_usec));
	
	printf("%17i\n",(int)(tv2.tv_usec - tv1.tv_usec));
//	write_data(fd,0x00);
//	getchar();
//	ioctl(fd,PPWCONTROL,0x55);
	ioperm(parportaddr,5,0);
	ioctl(fd,PPRELEASE);
	close(fd);
	return 0;
}
