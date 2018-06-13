#include "mbed.h"
#include "Sht31.h"
#include "pinmap.h"

AnalogIn analogValue1(A1);
AnalogIn analogValue2(A2);
DigitalOut out_min(PA_4);
DigitalOut out_hour(PA_5);

Sht31 sht31 = Sht31(I2C_SDA, I2C_SCL);
DigitalOut LED(PB_0); 

Serial pc(USBTX, USBRX); 

void hour_click(void);
void min_click(void);
void reset_zero(void);

float v_hour, v_min, v_out1, v_out2;
int now_hour, now_min;

int main()
{
		reset_zero();
		

		/*while(1) {
			v_hour = analogValue1 * 33.f;
			v_min = analogValue2  * 33.f;
			
			printf("v_hour = %0.2fV, v_min = %0.2fV.\r\n", v_hour, v_min);
			
			

			
			
			wait(0.2);
			LED = 1;
			out1=1;
			wait(0.07);
			LED = 0;
			out1=0;
			wait(0.08);
			LED = 1;
			out2=1;
			wait(0.07);
			LED = 0;
			out2=0;
			wait(0.08);
			
			
		}*/
}

void hour_click() {
	out_hour = 1;
	wait(0.05);
	out_hour = 0;
	wait(0.1);
}

void min_click() {
	out_min = 1;
	wait(0.05);
	out_min = 0;
	wait(0.1);
}

void reset_zero() {
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

