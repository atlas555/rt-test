#include <linux/version.h>
#include <linux/module.h>
#include <asm/io.h>

#include <linux/ipipe.h>

#include <linux/time.h>

#define BASEPORT 0x378
#define PAR_INT 7
        
static struct ipipe_domain this_domain;
int counter = 0;

void handler(unsigned irq) 
{
        printk(">>> PARALLEL PORT INT HANDLED: counter=%d\n", counter);
        counter++;
        ipipe_control_irq(PAR_INT,0,IPIPE_ENABLE_MASK);
        ipipe_propagate_irq(PAR_INT);
}

void domain_entry (void)
{
        int a,k;

        printk("Domain %s has start!\n",ipipe_current_domain->name);
        ipipe_virtualize_irq(ipipe_current_domain,PAR_INT,(ipipe_irq_handler_t)&handler,NULL,NULL,IPIPE_DYNAMIC_MASK);
        printk("handler has registered!\n");

        printk("Set interrupt mode of this irq\n") ;
        //set port to interrupt mode; pins are output
        outb_p(0x10, BASEPORT + 2); 
        ipipe_control_irq(PAR_INT,0,IPIPE_ENABLE_MASK);

        k = 10;
        while(k--)
        {                

          outb_p(0x00,BASEPORT);
          a = inb(BASEPORT);
          printk("ACK : %d\n",a);
         // udelay(5);

          outb_p(0xff,BASEPORT);        
          a = inb(BASEPORT);
          printk("ACK : %d\n",a);
        }
        
        for( ;; )
        ipipe_suspend_domain();
}
        
static int __init mod_init (void) 
{
    struct ipipe_domain_attr attr;
        ipipe_init_attr (&attr);
        attr.name     = "My_Domain";
        attr.priority = IPIPE_ROOT_PRIO + 1;
        attr.entry    = &domain_entry;
        return ipipe_register_domain(&this_domain,&attr);
}

static void __exit mod_exit (void) 
{
        ipipe_unregister_domain(&this_domain);
}

module_init(mod_init);
module_exit(mod_exit);
MODULE_LICENSE("GPL") ;