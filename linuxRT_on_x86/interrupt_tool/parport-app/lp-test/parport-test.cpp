//parport_linux.cpp

#include <stdio.h>
#include <sys/time.h>  // select()
#include <fcntl.h>   // open()
#include <unistd.h>    // read() write() close()
#include <signal.h>  // signal()
#include <termios.h>
#include <term.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdlib.h>     //exit()
#include <linux/ppdev.h>
#include <linux/parport.h>
#include <iostream>
#include <stdbool.h>
using namespace std;

#if 0
#define PARPORT_MODE_PCSPP (1<<0) /* IBM PC registers available. */
#define PARPORT_MODE_TRISTATE (1<<1) /* Can tristate. */
#define PARPORT_MODE_EPP (1<<2) /* Hardware EPP. */
#define PARPORT_MODE_ECP (1<<3) /* Hardware ECP. */
#define PARPORT_MODE_COMPAT (1<<4) /* Hardware 'printer protocol'. */
#define PARPORT_MODE_DMA (1<<5) /* Hardware can DMA. */
#define PARPORT_MODE_SAFEININT (1<<6) /* SPP registers accessible in IRQ. */

/* IEEE1284 modes:
      Nibble mode, byte mode, ECP, ECPRLE and EPP are their own
	     'extensibility request' values.  Others are special.
		    'Real' ECP modes must have the IEEE1284_MODE_ECP bit set.  */
#define IEEE1284_MODE_NIBBLE             0
#define IEEE1284_MODE_BYTE              (1<<0)
#define IEEE1284_MODE_COMPAT            (1<<8) //,Compatibility mode, Pin2-9, only used for output
#define IEEE1284_MODE_BECP              (1<<9) /* Bounded ECP mode */
#define IEEE1284_MODE_ECP               (1<<4)
#define IEEE1284_MODE_ECPRLE            (IEEE1284_MODE_ECP | (1<<5))
#define IEEE1284_MODE_ECPSWE            (1<<10) /* Software-emulated */
#define IEEE1284_MODE_EPP               (1<<6)
#define IEEE1284_MODE_EPPSL             (1<<11) /* EPP 1.7 */
#define IEEE1284_MODE_EPPSWE            (1<<12) /* Software-emulated */
#define IEEE1284_DEVICEID               (1<<2)  /* This is a flag */
#define IEEE1284_EXT_LINK               (1<<14) /* This flag causes the*/
#endif
unsigned int pp_cntl;
int fd;
static void User_Abort(int dummy)
{
	// Send_Command( STOP_TASK, 0, 0);
	// Display_Message(" Please turn off ");
	// Log_Stop("log.dat", LOG_FIELD);
	// Display_Close();
	 ioctl(fd, PPRELEASE); 
	 close(fd);
	 exit(0);
}


int main(void)
{
	 int ret_w;
	 int mode; /* We'll need this later. */
	 unsigned char status, control, data;
	 unsigned char outbuf[1]={0x55};
	 unsigned char mask = (PARPORT_STATUS_ERROR | PARPORT_STATUS_BUSY);
	 unsigned char val = (PARPORT_STATUS_ERROR | PARPORT_STATUS_BUSY);
	 int irqc;
	int busy ;//= nAck | nFault;
	int acking; // = nFault;
	int ready; // = Busy | nAck | nFault;
	char ch;

	struct timespec ts;
				 
	if( (fd = open("/dev/parport0", O_RDWR))<0)
		return 1;
	if (fd == -1) {
		perror ("open");
		return 1;
	}
				   
	if(ioctl (fd, PPEXCL)) { //register device exclusively
		perror ("PPCLAIM");
		close (fd);
		return 1;
	}
	 if(ioctl (fd, PPCLAIM)) {
		  perror ("PPCLAIM");
	    close (fd);
		  return 1;
	   }
		// mode = IEEE1284_MODE_COMPAT;
		 mode = PARPORT_MODE_PCSPP;// IBM PC registers available.
		  //PCSPP mode: pin2-9 only can be used for output
		 // mode = IEEE1284_MODE_BYTE;// IBM PC registers available.
		 // if (ioctl (fd, PPNEGOT, &mode)) {
	  if(ioctl(fd, PPSETMODE, &mode)) {
		    perror("PPNEGOT");
		  close (fd);
		    return 1;
		 }

		  //delay
	   ts.tv_sec = 0;
	    ts.tv_nsec = 0;
		 nanosleep(&ts, NULL);

	  ioctl(fd, PPRSTATUS, &status);
	  // if ((status & mask) == val) break;
	   data = 0x56; 
	    ioctl(fd, PPWDATA, &data); //write data
							 
	 getchar();
	  ready = 0x55;
	   ioctl(fd, PPWCONTROL, &ready);

	    ioctl(fd, PPRDATA, &data); //read
								 
        signal(SIGINT , User_Abort);
	 pp_cntl = PARPORT_CONTROL_SELECT;  //Low pin 17
		  // in include/linux/parport.h
		  // PARPORT_CONTROL_STROBE
	  // PARPORT_CONTROL_AUTOFD
										  // PARPORT_CONTROL_SELECT
										  // PARPORT_CONTROL_INIT

	  ioctl(fd, PPWCONTROL, &pp_cntl);
	   pp_cntl = ~PARPORT_CONTROL_SELECT;  //High pin 17
	    ioctl(fd, PPWCONTROL, &pp_cntl);
								 
	 ioctl(fd, PPRSTATUS, &status);
	   printf("\n %x", status>>6);
 


#if 0 

/* Set up the control lines when an interrupt happens. */
 ioctl (fd, PPWCTLONIRQ, &busy); /* Now we're ready. */
  ioctl (fd, PPWCONTROL, &ready);

  /*set Data line direction: non-zero for input mode*/
   ioctl (fd, PPDATADIR, &ch); 
   /* Fetch the data. */
    ioctl (fd, PPRDATA, &ch);  //read data
	/* Clear the interrupt. */
	 ioctl (fd, PPCLRIRQ, &irqc);
	  ioctl (fd, PPWCONTROL, &acking);
	   ioctl (fd, PPWCONTROL, &busy);
#endif 
	    ioctl(fd,PPRELEASE); 
		 close(fd);
}
