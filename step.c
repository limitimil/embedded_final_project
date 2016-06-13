#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "step.h"
#include "myMemMap.h"
#define BTN_ADDRESS   0x41210000
#define MOTOR_ADDRESS 0x41200000
#define SPEED 5000
void delay(){
	usleep(SPEED);
}
int step_ccw(){
	write_value(MOTOR_ADDRESS,12);
	delay();
	write_value(MOTOR_ADDRESS, 9);
	delay();
	write_value(MOTOR_ADDRESS, 3);
	delay();
	write_value(MOTOR_ADDRESS, 6);
	delay();
	write_value(MOTOR_ADDRESS,12);
	delay();
	return 0;
}
int step_cw(){
	write_value(MOTOR_ADDRESS,12);
	delay();
	write_value(MOTOR_ADDRESS, 6);
	delay();
	write_value(MOTOR_ADDRESS, 3);
	delay();
	write_value(MOTOR_ADDRESS, 9);
	delay();
	write_value(MOTOR_ADDRESS,12);
	delay();
	return 0;
}
int rotate360_cw(){
	int i;
	for(i=50;i!=0;i--) step_cw();
	return 0;
}
int rotate360_ccw(){
	int i;
	for(i=50;i!=0;i--) step_ccw();
	return 0;
}
void step_degree(int d) //range from -500~500
{
	int i;
	if(d > 0){
		for(i=d;i!=0;i--) step_cw();
	}
	else {
		for(i=-d;i!=0;i--) step_ccw();
	}
}
void step_prepare(){
	if(mem_prepare()!=0){
		exit(612);
	}
	write_value(MOTOR_ADDRESS,0xcc);
	step_degree(100);
}
void step_stop(){
	mem_close();
}
