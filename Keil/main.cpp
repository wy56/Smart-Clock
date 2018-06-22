#include "mbed.h"
#include "pinmap.h"

AnalogIn pinHourIn(A1);
AnalogIn pinMinIn(A2);
DigitalOut out_min(PA_4);
DigitalOut out_hour(PA_5);
Serial pc(USBTX, USBRX); 
Serial nodemcu(PC_6,PC_7);

int i, v_out1, v_out2;
float v_hour, v_min;
int now_hour = 0, now_min = 0;
double now_sec = 0;
int timeout_c = 0;

int X0,X1,X2,X3;
int gra_nodemcu_ready, gra_clock_reset,gra_clock_sync,gra_clock_count;
void grafcet();
void action();
char buffer[64];
void wait_clock(double);
void check_clock();
void hour_click(void);
void min_click(void);
void reset_clock(void);
void set_clock(int, int, int);


int main()
{
    nodemcu.baud(9600);
    pc.printf("Start Smart Clock!\n");
    X0=1;
    while(1){
        grafcet();
    }  
    
    
}
void grafcet(){
    if(X0==1 && gra_nodemcu_ready == 1)X0=gra_nodemcu_ready=0,X1=1;
    else if(X1==1 && gra_clock_reset == 1)X1=gra_clock_reset=0,X2=1;
    else if(X2==1 && gra_clock_sync == 1)X2=gra_clock_sync=0,X3=1;
    else if(X3==1 && gra_clock_count == 1)X3=gra_clock_count=0,X2=1;
    pc.printf("X0 = %d, X1 = %d, X2 = %d, X3 = %d\r\n",X0,X1,X2,X3);
    action();
}
void action(){
    if(X0){
        if (nodemcu.readable()) {               
            nodemcu.scanf("%s", buffer);
            pc.printf("%s\n\r", buffer);
            if(strcmp(buffer,"192.168.4.1")==0){
                pc.printf("gra_nodemcu_ready\n\r");
                gra_nodemcu_ready = 1;
            }
            
        }
    }else if(X1){
        reset_clock();
        pc.printf("gra_clock_reset\n\r");
        gra_clock_reset=1;
    }else if(X2){
        int time_out_max = 10000;
        if (nodemcu.readable()) {     
            int h;
            int m;
            int s;
            nodemcu.scanf("%s", buffer);
            pc.printf("%s\n\r", buffer);
            if(buffer[2]==':'&&buffer[5]==':'){
                pc.printf("Time:%s\n\r", buffer);
                sscanf(buffer, "%d:%d:%d", &h,&m,&s); 
                pc.printf("Time2:%d %d %d\n\r", h,m,s);                                      
            }
            if(buffer[3]==':'&&buffer[6]==':'){
                pc.printf("Time:%s\n\r", buffer);
                sscanf(buffer, "%d:%d:%d", &h,&m,&s); 
                h=h%100;
                pc.printf("Time2:%d %d %d\n\r", h,m,s);           
                if(h<24&&m<60&&s<60){
                    set_clock(h, m, s);
                }
            }
        }    
        pc.printf("gra_clock_sync\n\r");
        gra_clock_sync = 1;
    }else if(X3){
        now_sec += 1;
        check_clock();
        pc.printf("gra_clock_count\n\r");
        gra_clock_count=1;
    }
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
void check_clock(){
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
        v_hour = pinHourIn  * 33.f;
        hour_click();
    }
    
    while(v_hour>30) {
        hour_click();
        v_hour = pinHourIn  * 33.f;
    }
    
    while(v_min<30) {
        v_min = pinMinIn  * 33.f;
        min_click();
    }
    
    while(v_min>30) {
        min_click();
        v_min = pinMinIn  * 33.f;
    }
}

void set_clock(int input_hour, int input_min, int input_sec) {
    int old_hour = now_hour;
    int old_min = now_min;
    int old_sec = now_sec;
    now_hour = input_hour;
    now_min = input_min;
    now_sec = input_sec;
    int d_hour = now_hour - old_hour;
    int d_input = now_min - old_min;
    int d_sec = now_sec - old_sec;
    if(d_hour<0||d_input<0){
        reset_clock();
        old_hour = 0;
        old_min = 0;
    }
    
    for(i=old_hour; i<now_hour; i++) {
        if(i >= 12) {
            hour_click();
        }
        hour_click();
        hour_click();
    }
    for(i=old_min; i<now_min; i++) {
        min_click();
    }
    
}
