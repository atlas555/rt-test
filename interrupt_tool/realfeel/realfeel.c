//#include "/root/linux/include/linux/rtc.h"
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

double second() {
    struct timeval tv;
    gettimeofday(&tv,0);
    return tv.tv_sec + 1e-6 * tv.tv_usec;
}
typedef unsigned long long u64;
u64 rdtsc() {
    u64 tsc;
    __asm__ __volatile__("rdtsc" : "=A" (tsc));
    return tsc;
}
void selectsleep(unsigned us) {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = us;
    select(0,0,0,0,&tv);
}

double secondsPerTick,ticksPerSecond;

void calibrate() {
	//时间校准函数
    double sumx = 0;
    double sumy = 0;
    double sumxx = 0;
    double sumxy = 0;
    double slope;

    // least squares linear regression of ticks onto real time
    // as returned by gettimeofday.

    const unsigned n = 30;
    unsigned i;

    for (i=0; i<n; i++) {
	double breal,real,ticks;
	u64 bticks;
	
	breal = second();
	bticks = rdtsc(); //获取TSC值。X86平台采用rdtsc指令读取时间戳计数器完成高精度计时

	selectsleep((unsigned)(10000 + drand48() * 200000));

	ticks = rdtsc() - bticks;
	real = second() - breal;

	sumx += real;
	sumxx += real * real;
	sumxy += real * ticks;
	sumy += ticks;
    }
    slope = ((sumxy - (sumx*sumy) / n) /
	     (sumxx - (sumx*sumx) / n));
    ticksPerSecond = slope;
    secondsPerTick = 1.0 / slope; //每个tick占多少秒。
    printf("%3.3f MHz\n",ticksPerSecond*1e-6); //打印每秒多少tick，即主频数。
}
void fatal(char *msg) {
    perror(msg);
    exit(1);
}
int main() {
    calibrate();//时钟校准

    int fd = open("/dev/rtc",O_RDONLY);
	/*
	RTC:Real Time Clock硬件实时时钟，可产生周期信号,频率变化范围从2Hz到8192Hz.
	通过8号中断提交给系统（标准PC机的IRQ 8是如此设定的。
	/dev/rtc主设备号10，从设备号135，只读字符设备.对该设备执行读（read）操作，
	会得到unsigned long型的返回值，最低的一个字节表明中断的类型（更新完毕update-done，定时到达alarm-rang，周期信号periodic）；
	其余字节包含上次读操作以来中断到来的次数。
	如果系统支持/proc文件系统，/proc/driver/rtc中也能反映相同的状态信息。
	*/
    if (fd == -1) 
	fatal("failed to open /dev/rtc");

    int hz = 64;
    double ideal = 1.0 / hz;

    if (ioctl(fd, RTC_IRQP_SET, hz) == -1)
	fatal("ioctl(RTC_IRQP_SET) failed");

    printf("%d Hz\n",hz);

    /* Enable periodic interrupts */
    if (ioctl(fd, RTC_PIE_ON, 0) == -1)
	fatal("ioctl(RTC_PIE_ON) failed");

    u64 last = rdtsc();

    while (1) {
	u64 now;
	double delay;

	int data;
	if (read(fd, &data, sizeof(data)) == -1)
	    fatal("blocking read failed");

	now = rdtsc();
	delay = secondsPerTick * (now - last);
	printf(""%f\n",1e6 * delay");     // print delay;
//	printf("%f\n",1e6 * (ideal - delay));
	last = now;
    }
    if (ioctl(fd, RTC_PIE_OFF, 0) == -1)
	fatal("ioctl(RTC_PIE_OFF) failed");

    return 0;
}
