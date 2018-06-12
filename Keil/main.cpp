#include "mbed.h"
#include "Sht31.h"
#include "pinmap.h"

Sht31 sht31 = Sht31(I2C_SDA, I2C_SCL);
DigitalOut LED(PB_0); 
Serial pc(USBTX, USBRX); 

void initialization(void);
void grafect(void);
void action(void);
void printState(void);

int Input, X1, X2, X3, X4;
float temp, humid;

int main()
{
		initialization();
    while (true) {
			grafect();
			wait(1);
    }	
}

void initialization() {
		Input=1, X1=0, X2=0, X3=0, X4=0;
}

void grafect() {
		if(Input == 1) Input=0, X1=1;
		else if(X1 == 1) X1=0, X2=1;
		else if(X2 == 1 && ((int)temp > 28 || (int)humid > 60)) X2=0, X3=1;
		else if(X2 == 1 && (int)temp <= 28 && (int)humid <= 60) X2=0, X4=1;
		else if(X3 == 1) X3=0, Input=1;
	  else if(X4 == 1) X4=0, Input=1;
		action();
		printState();
}

void printState() {
		pc.printf("Input = %d, X1 = %d, X2 = %d, X3 = %d, X4 = %d\r\n", Input, X1, X2, X3, X4);
}

void action() {
		if(Input == 1) {
			temp = sht31.readTemperature();
		}
		else if(X1 == 1) {
			humid = sht31.readHumidity();
			pc.printf("~ Result of STH31-D Sensor ~\r\n");
			pc.printf("-------------------------------\r\n");
			pc.printf("Temperature : %.2f oC\r\n", temp);
			pc.printf("Humidity    : %.2f %%\r\n\r\n\r\n", humid);
		}
		else if(X2 == 1 && ((int)temp > 28 || (int)humid > 60)) {
			LED = 1;
			pc.printf("LED is on\r\n");
		}
		else if(X2 == 1 && (int)temp <= 28 && (int)humid <= 60) {
			LED = 0;
			pc.printf("LED is off\r\n");
		}
}
