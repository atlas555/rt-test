#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

#define PORTADDRESS 0x378
#define DATA        (PORTADDRESS + 0)
#define STATUS      (PORTADDRESS + 1)
#define CONTROL     (PORTADDRESS + 2)

#define CMDWRITE    0x00
#define CMDREAD     0x21

int main(int argc, char * argv[])
{
	int ioData = 0;	

	while(1)
	{
		if(ioperm(PORTADDRESS, 3, 1)) exit(1);
		
		outb(CMDREAD, CONTROL);
		ioData = inb(DATA);
		printf("0x%x\n", ioData);

		if(ioperm(PORTADDRESS, 3, 0)) exit(1);
	}

	return 0;
}
