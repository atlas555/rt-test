
/*
 * interrupt_latency_x86 v1.0 11/25/01
 * www.embeddedlinuxinterfacing.com
 *
 * The original location of this code is
 * http://www.embeddedlinuxinterfacing.com/chapters/11/
 *
 * Copyright (C) 2001 by Craig Hollabaugh
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
 * interrupt_latency_x86.c is based on procfs_example.c by Erik Mouw.
 * For more information, please see, The Linux Kernel Procfs Guide, Erik Mouw
 * http://kernelnewbies.org/documents/kdoc/procfs-guide/lkprocfsguide.html
 */

/*
gcc -O2 -D__KERNEL__ -DMODULE -I/usr/src/linux/include \
-c interrupt_latency_x86.c -o interrupt_latency_x86.o
*/

/* interrupt_latency_x86.c module
 * This module measures interrupt latency of x86 machines by connecting
 * parallel printer port D7 (pin 9) to ACK (pin 10). Enable interrupt
 * generation by status register configuration. Positive going edge on
 * ACK pin generates interrupt 7.
 *
 * Kernel source claims microsecond resolution of do_gettimeofday. Viewing
 * the D7-ACK connection verifies this, well we're close with 10uS.
 *
 * After module insertion, reading /proc/interrupt_latency will assert D7
 * generating the interrupt. The interrupt handler will deassert this signal.
 * View on scope. An interrupt counter is included to help debug a noisy
 * interrupt line.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>

#include <asm/io.h> /* outb */
#define MODULE_VERSION "1.0"
#define MODULE_NAME "interrupt_latency_x86"

int interruptcount = 0;
struct timeval tv1, tv2; /* do_gettimeofday fills these */

#define SPPDATAPORT         0x378
#define SPPSTATUSPORT       (SPPDATAPORT + 1)
#define SPPCONTROLPORT      (SPPDATAPORT + 2)
#define SSPINTERRUPTENABLE  0x10

#define INTERRUPT 7

static struct proc_dir_entry *interrupt_latency_file;

/*
 * function interrupt_interrupt_latency
 * This function is the interrupt handler for interrupt 7. It sets the tv2
 * structure using do_gettimeofday. It then deasserts D7.
 */
void interrupt_interrupt_latency(int irq, void *dev_id, struct pt_regs *regs)
{
  do_gettimeofday(&tv2);
  outb(0x00,SPPDATAPORT); /* deassert the interrupt signal */
  interruptcount++;
}

/*
 * function proc_read_interrupt_latency
 * The kernel executes this function when a read operation occurs on
 * /proc/interrupt_latency. This function sets the tv1 structure. It asserts
 * D7 which should immediately cause interrupt 7 to occur. The handler
 * records tv2 and deasserts D7. This function returns the time differential
 * between tv2 and tv1.
 */
static int proc_read_interrupt_latency(char *page, char **start, off_t off,
                                       int count, int *eof, void *data)
{
  int len;
  do_gettimeofday(&tv1);
  outb(0x80,SPPDATAPORT); /* assert the interrupt signal */


  len = sprintf(page, "Start   %9i.%06i\nFinish  %9i.%06i\nLatency %17i\n\
Count %19i\n",(int) tv1.tv_sec, (int) tv1.tv_usec,
(int) tv2.tv_sec, (int) tv2.tv_usec, (int) (tv2.tv_usec - tv1.tv_usec),
interruptcount);

  return len;
}

/*
 * function init_interrupt_latency
 * This function creates the /proc directory entry interrupt_latency. It
 * also configures the parallel port then requests interrupt 7 from Linux.
 */
static int __init init_interrupt_latency(void)
{
  int rv = 0;

  interrupt_latency_file = create_proc_entry("interrupt_latency", 0444, NULL);
  if(interrupt_latency_file == NULL) {
    return -ENOMEM;
  }

  interrupt_latency_file->data = NULL;
  interrupt_latency_file->read_proc = &proc_read_interrupt_latency;
  interrupt_latency_file->write_proc = NULL;
  interrupt_latency_file->owner = THIS_MODULE;

  /* request interrupt from linux */
  rv = request_irq(INTERRUPT, interrupt_interrupt_latency, 0,
                   "interrupt_latency",NULL);
  if ( rv ) {
    printk("Can't get interrupt %d\n", INTERRUPT);
    goto no_interrupt_latency;
  }

/* enable parallel port interrupt generation */
  outb(SSPINTERRUPTENABLE,SPPCONTROLPORT);

/* deassert the interrupt signal */
  outb(0x00,SPPDATAPORT);

/* everything initialized */
  printk(KERN_INFO "%s %s initialized\n",MODULE_NAME, MODULE_VERSION);
  return 0;

/* remove the proc entry on error */
  no_interrupt_latency:
  remove_proc_entry("interrupt_latency", NULL);
}


/*
 * function cleanup_interrupt_latency
 * This function frees interrupt 7 then removes the /proc directory entry
 * interrupt_latency.
 */
static void __exit cleanup_interrupt_latency(void)
{
/* disable parallel port interrupt reporting */
  outb(0x00,SPPCONTROLPORT);

/* free the interrupt */
  free_irq(INTERRUPT,NULL);

  remove_proc_entry("interrupt_latency", NULL);

  printk(KERN_INFO "%s %s removed\n", MODULE_NAME, MODULE_VERSION);
}

module_init(init_interrupt_latency);
module_exit(cleanup_interrupt_latency);

MODULE_AUTHOR("Craig Hollabaugh");
MODULE_DESCRIPTION("interrupt_latency proc module");

EXPORT_NO_SYMBOLS;

