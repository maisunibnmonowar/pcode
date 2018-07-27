#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "SimpleGPIO.h"

using namespace std;

//definition 
//define full verbose 
#define vv

unsigned int CE		= 48;//P9_15
unsigned int SLE 	= 69;
unsigned int SDATA 	= 44;
unsigned int SREAD 	= 66;
unsigned int SCLK 	= 68;

//variables
bool isOn;
bool txMode;
unsigned int db[32]; //32 element long
//set
unsigned int T = 24000; //time period of register clock in us
int i;

//-------------------
//Algorithm

void powerUp(){
	gpio_set_value(CE, HIGH);//CE = 1
	usleep(100000); // in mS. 
	isOn = true;
	cout <<"on";
}

void powerDown(){
	gpio_set_value(CE, LOW);//CE = 0
	isOn = false;
	cout << "off";
}
void Initialize(){
	//turn on the ADF7021
	if(!isOn){
		powerUp();
		
	}

	//necessary reg operation
	//activate Rx mode
}

void regClock(){
	gpio_set_value(SCLK, LOW);
	usleep(T/2); //generate low half of the clk
	gpio_set_value(SCLK, HIGH); //generate high part of clk
	usleep(T/4); //but not in full

}
void clearDB(){
	cout << "clear db" <<endl;
	for(i = 0; i<32; i++){
		db[i] = 0;
	}
}
void readReg(){
	cout << "read reg" << endl;
	clearDB();
	gpio_set_value(SLE, HIGH);
	for(i = 17; i>=0; i--){//not sure how many bits are supposed to come in
		regClock();
		gpio_get_value(SREAD, &db[i]);
	}
	//SLE = 0;
	gpio_set_value(SLE, LOW);
}
void readSiliconRevision(){
	cout << "readSiliconRevision" << endl;
	//make sure everything is on
	if(!isOn){
		powerUp();
	}
	//set reg value
	clearDB();
	db[8] = 1;
	db[7] = 1;
	db[6] = 1;
	db[5] = 1;
	db[4] = 1;
	db[3] = 0;
	db[2] = 1;
	db[1] = 1;
	db[0] = 1;
	//ask for Revision code
	gpio_set_value(SLE, LOW);//SLE = 0;
	for(i = 8; i>=0; i--){
		gpio_set_value_2(SDATA, db[i]);
		regClock();
	}
	usleep(40000); //40 mS
	//read the Revision code
	readReg();
	
	//calculate the value
	unsigned int revisionCode = (((2^3)*db[4])+((2^2)*db[3])+((2^1)*db[2])+((2^0)*db[1]))				;
	unsigned int productCode = 	((100*	(((2^3)*db[16])+((2^2)*db[15])+((2^1)*db[14])+((2^0)*db[13]))) 	+
								(10*	(((2^3)*db[12])+((2^2)*db[11])+((2^1)*db[10])+((2^0)*db[9]))) 	+
								(1*		(((2^3)*db[8])+((2^2)*db[7])+((2^1)*db[6])+((2^0)*db[5]))))		;
	//show the user the value
	cout <<"Revision code: "<< std::hex << revisionCode << endl;
	cout <<"Product code : "<< std::hex << productCode << endl;
}

int main(){
	//pin direction and initial GPIO level
/*	//CE
	system("config-pin p9.15 gpio_pd");
	system("config-pin p9.15 out");
	system("config-pin -q p9.15");
	//SLE
	system("config-pin p8.09 gpio_pd");
	system("config-pin p8.09 out");
	system("config-pin -q p8.09");
	//SDATA
	system("config-pin p8.12 gpio_pd");
	system("config-pin p8.12 out");
	system("config-pin -q p8.12");
	//SREAD
	system("config-pin p8.07 gpio_pd");
	system("config-pin p8.07 in");
	system("config-pin -q p8.07");
	//SCLK
	system("config-pin p8.10 gpio_pd");
	system("config-pin p8.10 out");
	system("config-pin -q p8.10");
*/
	gpio_export(CE);
	gpio_export(SLE);
	gpio_export(SDATA);
	gpio_export(SREAD);
	gpio_export(SCLK);
	
	gpio_set_dir(CE, OUTPUT_PIN);
	gpio_set_dir(SLE, OUTPUT_PIN); 
	gpio_set_dir(SDATA, OUTPUT_PIN);   // The LED is an output
	gpio_set_dir(SCLK, OUTPUT_PIN);   // The LED is an output
	gpio_set_dir(SREAD, INPUT_PIN);   // The LED is an output
	//do something
	readSiliconRevision();
	//power down
	powerDown();
	//unexport pins
	gpio_unexport(CE);     // unexport the LED
	gpio_unexport(SDATA);
	gpio_unexport(SREAD);
	gpio_unexport(SLE);
	gpio_unexport(SCLK);

	cout << "All done" << endl;
	
	//return
	return 0;
}