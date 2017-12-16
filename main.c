// 240 x 320

/*
avr-gcc -mmcu=atmega644p -s -DF_CPU=12000000 -Wall -Os main.c -o main.elf
avr-objcopy -O ihex main.elf main.hex
avrdude -c usbasp -p m644p -U flash:w:main.hex

LCD connected across port A/C
*/
#define UART_BAUD 38400

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "pictor.h"

#include "toastrack_ser.h"
//void serialGet(uint8_t send_byte, uint8_t recv_count, uint8_t* buffer)

#include "font.h"

// remove this used to get random numbers for testing
#include <stdlib.h>

static const unsigned char text0[] PROGMEM = "User Output 1";
static const unsigned char text1[] PROGMEM = "User Output 2";
static const unsigned char text2[] PROGMEM = "User Output 3";
static const unsigned char text3[] PROGMEM = "User Output 4";
static const unsigned char text4[] PROGMEM = "Shift Indicator";

void init_mode0();
void init_mode1();

int outputBox[5] = {140, 160, 180, 200, 220};

int main(){
	pictorInit(0);
	pictorSetRotation(3);

	initUart();

	
	uint16_t RPM = 0;
	uint16_t prevPercent = 0;
	uint16_t percent = 0;

	uint8_t mode = 0;
	uint8_t oldMode = 0;
	init_mode0();

	while(1){
		if(mode == 0){
			if(oldMode != 0){
				init_mode0();
				oldMode = 0;
			}

			char values[9]; 
			//serialGet('S', 9, values);

			//uint8_t advance = values[0];
			uint8_t advance = (rand() >> 9);

			uint16_t rawRPM = (values[1] << 8) | values[2];
			//uint16_t RPM = 30000000/rawRPM;
			RPM = (rand() >> 4);	//temp function to get random number for RPM testing

			//char outState = values[5];
			char outState = 0b00101111;

			percent = 310.0 * (RPM/2500.0);
			if(percent > 310){ percent = 310; }

			int16_t diff = percent - prevPercent;
			if(diff > 0){
				pictorDrawBox((point){prevPercent + 10, 25}, (point){percent + 10, 55}, WHITE);
			}
			else if(diff < 0) {
				pictorDrawBox((point){percent + 11, 3}, (point){prevPercent + 10, 55}, BLACK);
			}
			prevPercent = percent;

			// draw RPM
			pictorDrawD(RPM, (point){10, 100}, WHITE, BLACK, Mash, 4, 4);

			// draw advance value
			pictorDrawD(advance, (point){150, 100}, WHITE, BLACK, Mash, 4, 4);

			// output states
			uint8_t i, j;				
			for(i = 0; i < 6; i++){
				if(i != 4){
					if(outState & (1 << i)){
						j = (i == 5) ? (4) : (i); 
						pictorDrawBox((point){22, outputBox[j] + 2}, (point){26, outputBox[j] + 6}, WHITE);
					}
					else{
						pictorDrawBox((point){22, outputBox[j] + 2}, (point){26, outputBox[j] + 6}, BLACK);
					}
				}
			}
		}
		else if(mode == 1){
			if(oldMode != 1){
				init_mode1();
			}
			oldMode = 1;
		}
	}

		//_delay_ms(2);



}

void init_mode0(){
	pictorDrawBox((point){10, 59}, (point){310,60}, WHITE);

	int locations[6] = {10, 69, 129, 189, 249, 309};
	int labels[6] = {0, 500, 1000, 1500, 2000, 2500};
	int i = 0;
	for(i; i < 6; i++){
		pictorDrawBox((point){locations[i], 60}, (point){locations[i] + 1, 70}, WHITE);
		pictorDrawD(labels[i], (point){locations[i] - 22, 75}, WHITE, BLACK, Mash, 1, 4);
	}

	for(i = 0; i < 5; i++){
		pictorDrawBox((point){20, outputBox[i]}, (point){28, outputBox[i] + 8}, WHITE);
		pictorDrawBox((point){22, outputBox[i] + 2}, (point){26, outputBox[i] + 6}, BLACK);
	}
	pictorDrawSP(text0, (point){32, outputBox[0] + 2}, WHITE, BLACK, Mash, 1);
	pictorDrawSP(text1, (point){32, outputBox[1] + 2}, WHITE, BLACK, Mash, 1);
	pictorDrawSP(text2, (point){32, outputBox[2] + 2}, WHITE, BLACK, Mash, 1);
	pictorDrawSP(text3, (point){32, outputBox[3] + 2}, WHITE, BLACK, Mash, 1);
	pictorDrawSP(text4, (point){32, outputBox[4] + 2}, WHITE, BLACK, Mash, 1);
}

void init_mode1(){
	char values[150]; 
	//serialGet('C', 150, values);
	#define RPM_DRAW_SCALE 1
	char* RPMbins = &values[0];
	char* Loadbins = &values[10];

	char* Ignmap = &values[20];

	point Pos = (point){5,(8*RPM_DRAW_SCALE)+7};
	pictorDrawAll(WHITE);
	uint8_t i;
	for(i=0; i<150; i++){
		values[i] = 32;
	}
	for(i=0; i<9; i++){
		pictorDrawD(Loadbins[i], Pos, WHITE, BLUE, Mash, RPM_DRAW_SCALE, 2);
		Pos.Y += (8*RPM_DRAW_SCALE)+2;
	}
	Pos = (point){(RPM_DRAW_SCALE*2*8)+7,5};
	for(i=0; i<9; i++){
		pictorDrawD(RPMbins[i], Pos, WHITE, BLUE, Mash, RPM_DRAW_SCALE, 2);
		Pos.X += (RPM_DRAW_SCALE*2*8)+2;
	}
	Pos = (point){(RPM_DRAW_SCALE*2*8)+7,(8*RPM_DRAW_SCALE)+7};
	uint8_t j;
	for(j=0; j<9; j++){
		for (i=0; i<9; i++){
			pictorDrawD(Ignmap[i+j*10], Pos, WHITE, BLACK, Mash, RPM_DRAW_SCALE, 2);
			Pos.X += (RPM_DRAW_SCALE*2*8)+2;
		}
		Pos.Y += (8*RPM_DRAW_SCALE)+2;
		Pos.X = (8*RPM_DRAW_SCALE*2)+7;
	}
}
