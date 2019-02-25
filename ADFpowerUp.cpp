#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "SimpleGPIO.h"

using namespace std;

//definition 
//define full verbose 
//#define vv
#define dotLength 60000 //60 mS

unsigned int CE		= 48;//P9_15
unsigned int SLE 	= 69;
unsigned int SDATA 	= 44;
unsigned int SREAD 	= 66;
unsigned int SCLK 	= 68;
unsigned int DUE	= 47; //P8.15 OR GPIO 47
//unsigned int MUXOUT = 13;

//variables
bool isOn;
bool txMode;
unsigned int db[32]; //32 element long
unsigned int revisionCode;
unsigned int productCode;
unsigned long controlWord;
unsigned char iterationTime;

//set
unsigned int T = 24000; //time period of register clock in us
signed int i;
int verboseLevel = 0; // 0 being quiet, 2 being full verbose

//-------------------
//Algorithm

void powerUp()
{
	gpio_set_value(CE, HIGH);//CE = 1
	usleep(5000); // 5 mS. 
	isOn = true;
	if(verboseLevel > 0){cout <<"on"<< endl;}
}

void powerDown()
{
	cout << "Powering down" << endl;
	gpio_set_value(CE, LOW);//CE = 0
	isOn = false;
	cout << "off"<< endl;
}
void Initialize()
{
	cout << "Initializing" << endl;
	//turn on the ADF7021
	if(!isOn){
		powerUp();
		
	}

	//necessary reg operation
	//activate Rx mode
}

void regClock()
{
	gpio_set_value(SCLK, LOW);
	usleep(T/2); //generate low half of the clk
	gpio_set_value(SCLK, HIGH); //generate high part of clk
	usleep(T/4); //but not in full

}
void clearDB(){
	if(verboseLevel > 1){cout << "clear db" <<endl;}
	for(i = 0; i<32; i++){
		db[i] = 0;
	}
	//Alternate code
	//memset(db, 0, sizeof(db));
}

void readReg()
{
	if(verboseLevel > 1){cout << "read reg" << endl;}
	clearDB();
	gpio_set_value(SLE, HIGH);
	for(i = 17; i>=0; i--)
	{
		regClock();
		gpio_get_value(SREAD, &db[i]);
	}
	usleep(T/4);
	//SLE = 0;
	gpio_set_value(SLE, LOW);
	if(verboseLevel > 0){cout << "read Reg finish" << endl;}
}
void sendReg()
{
	cout << "Send reg" << endl;
	//make sure everything is on
	if(!isOn){
		powerUp();
	}
	//MUXOUT must be HIGH for Programming to begin
	//while(gpio_get_value(MUXOUT) == LOW))
	//{
		//wait
	//}
	//Programming can now begin
	gpio_set_value(SLE, LOW);//SLE = 0;
	usleep(400000); //400 mS
	for(i = 31; i>=0; i--){
		gpio_set_value_2(SDATA, db[i]);
		if (verboseLevel > 1) {cout << "asdf sending " << i << "  "<<db[i] << endl;}
		regClock();
	}
	//while(i>=0)
	usleep(40000); //40 mS
	cout << "Send Reg finish" << endl;
}

void setReg(unsigned long word)
{
	if(verboseLevel > 0) {cout << "Set Reg" << endl;}
	//make sure everything is on
	if(!isOn){
		powerUp();
	}
	//make sure no residual value
	clearDB();
	//load value 
	for (i=0; word> 0; i++)
    {
        db[i] = word % 2;
        if(verboseLevel > 1) {cout << "asdf  loading" << i << "  "<<db[i] << endl;}
        word = word / 2;
    }
	//set value
	sendReg();
	usleep(1000); //1 mS
	if(verboseLevel > 0) {cout << "Set Reg finish" << endl;}
}

void setReg0()
{
	if(verboseLevel > 0) {cout << "Set reg 0" << endl;}
	setReg(0x1160D870);
	usleep(500); //500 uS
	if(verboseLevel > 0) {cout << "Set Reg 0 finish" << endl;}
}
void setReg1()
{
	if(verboseLevel > 0) {cout << "Set reg 1" << endl;}
	setReg(0x479011);
	usleep(5000); //5 mS
	if(verboseLevel > 0) {cout << "Set Reg 1 finish" << endl;}
}
void setReg2()
{
	if(verboseLevel > 0) {cout << "Set reg 2" << endl;}
	setReg(0xffef4c2);
	usleep(1000000); //1 S
	if(verboseLevel > 0) {cout << "Set Reg 2 finish" << endl;}
}
void setReg3()
{
	if(verboseLevel > 0) {cout << "Set reg 3" << endl;}
	setReg(0x2B160123);
	usleep(5000); //5 mS
	if(verboseLevel > 0) {cout << "Set Reg 3 finish" << endl;}
}
void setReg7()
{
	if(verboseLevel > 0) {cout << "Set reg 7" << endl;}
	setReg(0x1F7);
	if(verboseLevel > 0) {cout << "Set Reg 7 finish" << endl;}
}
void gpio_init()
{
	cout << "GPIO INIT" << endl;
	gpio_export(CE);
	gpio_export(SLE);
	gpio_export(SDATA);
	gpio_export(SREAD);
	gpio_export(SCLK);
	gpio_export(DUE);
	//gpio_export(MUXOUT);
	
	gpio_set_dir(CE, OUTPUT_PIN);
	gpio_set_dir(SLE, OUTPUT_PIN); 
	gpio_set_dir(SDATA, OUTPUT_PIN);  
	gpio_set_dir(SCLK, OUTPUT_PIN);   
	gpio_set_dir(SREAD, INPUT_PIN);   
	gpio_set_dir(DUE, OUTPUT_PIN);
	//gpio_set_dir(MUXOUT, INPUT_PIN);

	gpio_set_value(DUE, HIGH);
	cout << "GPIO INIT finish" << endl;
}

void gpio_release()
{
	cout << "GPIO release " << endl;
	//unexport pins
	gpio_unexport(CE);     // unexport the LED
	gpio_unexport(SDATA);
	gpio_unexport(SREAD);
	gpio_unexport(SLE);
	gpio_unexport(SCLK);
	gpio_unexport(DUE);
	//gpio_unexport(MUXOUT);
	cout << "GPIO Release finish" << endl;
}

void rx_mode()
{
	cout << "RX MODE" << endl;
	// 1 3 6? 5 11? 12? 0 4 10?
	
	//0
	setReg(0x8AC1B200);
}

void tx_mode() 
{
	if(verboseLevel > 0) {cout << "TX MODE" << endl;}
	//1 3 0 2
	setReg1();
	setReg3();
	setReg0();
	setReg2();

}
void dot()
{
	gpio_set_value(CE, HIGH);
	usleep(dotLength);
	gpio_set_value(CE, LOW);
	usleep(dotLength);
}

void dash()
{
	gpio_set_value(CE, HIGH);
	usleep(3*dotLength);
	gpio_set_value(CE, LOW);
	usleep(dotLength);
}
void space()
{
	gpio_set_value(CE, LOW);
	usleep(2*dotLength);// 120 mS
}

void J()
{
	dot();	dash();	dash();	dash();space();
}
void G()
{
	dash();	dash();	dot();space();
}
void M()
{
	dash();	dash();space();
}
void N()
{
	dash();	dot();space();
}
void B()
{
	dash();	dot();	dot();	dot();space();
}
void callSign()
{
	space(); space(); J();	G();	M();	N();	B();space();space();
}
void readSiliconRevisionV2(){
	setReg7();
	readReg();
	//calculate the value
	revisionCode = 				8*db[4]+	4*db[3]+	2*db[2]+	db[1]		;
	productCode = 	(100*(		8*db[16]+	4*db[15]+	2*db[14]+	db[13])) 	+
					(10*(		8*db[12]+	4*db[11]+	2*db[10]+	db[9])) 	+
					(1*	(		8*db[8]+	4*db[7]+	2*db[6]+	db[5]))		;
	//show the user the value
	cout <<"Revision code: "<< std::hex << "0x" << revisionCode << "     " << std::dec << revisionCode << endl;
	cout <<"Product code : "<< std::hex << "0x" << productCode  << "   " << std::dec << productCode << endl;

}
void testToneOnly()
{
	setReg(0x10F);
	cout <<"Transmiting Tone only"<< endl;
}

void enableSPI()
{
	setReg(0x000E000F);
	cout << "set spi";
}

void enableUART()
{
	setReg(0x0000000F);
	if(verboseLevel > 0){cout <<"UART Enabled"<<endl;}
}

void ardOn()
{
	system("config-pin -q p8.15");
	gpio_set_value(DUE, LOW);
	cout << "ard on";
}

void ardOff()
{
	gpio_set_value(DUE, HIGH);
	cout << "ard off";
}

int main(int argc, char *argv[]){
	if(argc>1)
	{
		for(i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "-on") 	== 0)		{		gpio_init();			}
			if(strcmp(argv[i], "-off") 	== 0)		{		powerDown();			gpio_release();	}
			if(strcmp(argv[i], "-cw") 	== 0)		{		callSign();			}
			if(strcmp(argv[i], "-v") 	== 0)		{		//define verbose minimum
											verboseLevel = 1;		}
			if(strcmp(argv[i], "-vv") 	== 0)		{		//define full verbose
											verboseLevel = 2;		}
			if(strcmp(argv[i], "-tx") 	== 0)		{		tx_mode();			}
			if(strcmp(argv[i], "-test") 	== 0)		{		testToneOnly();			}
			if(strcmp(argv[i], "-rev") 	== 0)		{		readSiliconRevisionV2();	}
			if(strcmp(argv[i], "-ardOn") 	== 0)		{		ardOn();			}
			if(strcmp(argv[i], "-ardOff") 	== 0)		{		ardOff();			}
			if(strcmp(argv[i], "-cw3") 	== 0)		{		callSign(); 			space();
											callSign();			space();
											callSign(); 			space();			}
			if(strcmp(argv[i], "-uart") 	== 0)		{		enableUART();			}
			if(strcmp(argv[i], "-spi") 	== 0)		{		enableSPI();			}
		}
	}
	powerUp();
	return 0;
}
