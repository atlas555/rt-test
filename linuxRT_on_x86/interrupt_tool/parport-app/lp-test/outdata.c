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
	int ioData = 0xff;
//	int ioControlBit1Pin14 = 0;
//	int ioControlBit2Pin16 = 0;
//	int ioControlBit3Pin17 = 0;
//	int pin14 = 0;
//	int pin16 = 0;
//	int pin17 = 0;
/*
	if(argc < 5)
	{
		printf("%s\n", "ERROR111!");
		return -1;
	}

*/
	if(ioperm(PORTADDRESS, 3, 1)) exit(1);

	outb(CMDWRITE, CONTROL);
	outb(ioData, DATA);


	if(ioperm(PORTADDRESS, 3, 0)) exit(1);

	return 0;
}

