#include <linux/version.h>
#include <linux/module.h>
#include <asm/io.h>

#define BASEPORT 0x378
#define PAR_INT 7

static adomain_t this_domain;
static adsysinfo_t sys_info;
int parcounter = 0;
long long timercounter = 0;

void handler(unsigned irq) {
        printk(">>> PARALLEL PORT INT HANDLED: counter=%d\n", parcounter);
        parcounter++;
        adeos_control_irq(PAR_INT,0,IPIPE_ENABLE_MASK);
        adeos_propagate_irq(irq);
}
void timer_tick (unsigned irq) {
        timercounter++;
        if (timercounter < 10) {
                outb_p(0, BASEPORT);
                outb_p(255, BASEPORT);
        }
        adeos_propagate_irq(irq);
}
void domain_entry (int iflag) {
        printk("Domain %s started.\n",adp_current->name);

        if (iflag) {
                adeos_get_sysinfo(&sys_info);
                // tmirq = timer irq
                adeos_virtualize_irq(sys_info.archdep.tmirq,
                        &timer_tick, NULL, IPIPE_DYNAMIC_MASK);
                // parallel port irq
                adeos_virtualize_irq(PAR_INT,&handler,NULL,IPIPE_DYNAMIC_MASK);
                //set port to interrupt mode; pins are output
                outb_p(0x10, BASEPORT + 2); 
                adeos_control_irq(PAR_INT,0,IPIPE_ENABLE_MASK);
        }

        for (;)
                // This domain's idle loop
                adeos_suspend_domain(); // control back to ADEOS
}
        
static int __init mod_init (void) {
        adattr_t attr;
        attr.name = "TestDomain";
        attr.domid = 1;             // Adeos Domain ID: >0
        attr.entry = &domain_entry;
        attr.estacksz = 0;  // Adeos chooses a reasonable stack size
        attr.priority = ADEOS_ROOT_PRI + 1;
        attr.dswitch = NULL;    // Domain switch hook - always a C routine
        return adeos_register_domain(&this_domain,&attr);
}

static void __exit mod_exit (void) {
        adeos_unregister_domain(&this_domain);
}

module_init(mod_init);
module_exit(mod_exit);
MODULE_LICENSE("GPL");