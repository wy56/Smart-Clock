#include "mbed.h"
#include "Sht31.h"
#include "pinmap.h"

AnalogIn analogValue1(A1);
AnalogIn analogValue2(A2);
DigitalOut out_min(PA_4);
DigitalOut out_hour(PA_5);
Sht31 sht31 = Sht31(I2C_SDA, I2C_SCL);
Serial pc(USBTX, USBRX); 

int i, v_out1, v_out2;
float v_hour, v_min;
int now_hour = 19, now_min = 41;
double now_sec = 12;

void wait_clock(double);
void hour_click(void);
void min_click(void);
void reset_clock(void);
void set_clock(int, int);

int main()
{
		set_clock(now_hour, now_min);
		
		while(1) {
			wait_clock(60.f - now_sec);
		}

		/*while(1) {
			v_hour = analogValue1 * 33.f;
			v_min = analogValue2  * 33.f;
			
			printf("v_hour = %0.2fV, v_min = %0.2fV.\r\n", v_hour, v_min);
			
		*/
}

void wait_clock(double sec) {
	wait(sec);
	now_sec += sec;
	if(now_sec >= 60.f) {
		now_sec -= 60;
		now_min += 1;
		min_click();
		if(now_min >= 60) {
			hour_click();
			hour_click();
			now_min -= 60;
			now_hour = (now_hour + 1) % 24;
			if(now_hour >= 12) {
				hour_click();
			}
		}
	}
}

void hour_click() {
	out_hour = 1;
	wait_clock(0.05);
	out_hour = 0;
	wait_clock(0.1);
}

void min_click() {
	out_min = 1;
	wait_clock(0.05);
	out_min = 0;
	wait_clock(0.1);
}

void reset_clock() {
	while(v_hour<30) {
		v_hour = analogValue1 * 33.f;
		hour_click();
	}
	
	while(v_hour>30) {
		hour_click();
		v_hour = analogValue1  * 33.f;	
	}

	while(v_min<30) {
		v_min = analogValue2  * 33.f;
		min_click();
	}
	
	while(v_min>30) {
		min_click();
		v_min = analogValue2  * 33.f;
	}
}

void set_clock(int now_hour, int now_min) {
	reset_clock();
	for(i=0; i<now_hour; i++) {
		if(i >= 12) {
			hour_click();
		}
		hour_click();
		hour_click();
	}
	for(i=0; i<now_min; i++) {
		min_click();
	}
}
