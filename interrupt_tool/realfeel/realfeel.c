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
	//ʱ��У׼����
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
	bticks = rdtsc(); //��ȡTSCֵ��X86ƽ̨����rdtscָ���ȡʱ�����������ɸ߾��ȼ�ʱ

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
    secondsPerTick = 1.0 / slope; //ÿ��tickռ�����롣
    printf("%3.3f MHz\n",ticksPerSecond*1e-6); //��ӡÿ�����tick������Ƶ����
}
void fatal(char *msg) {
    perror(msg);
    exit(1);
}
int main() {
    calibrate();//ʱ��У׼

    int fd = open("/dev/rtc",O_RDONLY);
	/*
	RTC:Real Time ClockӲ��ʵʱʱ�ӣ��ɲ��������ź�,Ƶ�ʱ仯��Χ��2Hz��8192Hz.
	ͨ��8���ж��ύ��ϵͳ����׼PC����IRQ 8������趨�ġ�
	/dev/rtc���豸��10�����豸��135��ֻ���ַ��豸.�Ը��豸ִ�ж���read��������
	��õ�unsigned long�͵ķ���ֵ����͵�һ���ֽڱ����жϵ����ͣ��������update-done����ʱ����alarm-rang�������ź�periodic����
	�����ֽڰ����ϴζ����������жϵ����Ĵ�����
	���ϵͳ֧��/proc�ļ�ϵͳ��/proc/driver/rtc��Ҳ�ܷ�ӳ��ͬ��״̬��Ϣ��
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
