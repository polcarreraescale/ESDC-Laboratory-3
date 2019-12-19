/*
 * version3.c demo code
 * The main code activates two state machines, that run concurrently. There is no scheduller.
 * State machines have to 
 *
 *  Created on: 12/2/2019
 *      Author: Josep Altet
*/

#include "xparameters.h"
#include "xgpio.h"
#include <stdio.h>
#include "xparameters.h"
//#include "netif/xadapter.h"
#include "platform_config.h"
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "connection.h"
#include "common.h"

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_THREAD_PRIO             1
#define DEFAULT_THREAD_STACKSIZE        200

#define BACKGROUND 0
#define SQUARE 4
#define FOOD 2
#define N_max 20

#define RIGHT 1
#define LEFT 2
#define UP 4
#define DOWN 8

#define RIGHT_FLIP 2
#define LEFT_FLIP 4

#define LX_FOOD 8
#define LY_FOOD 8

#define SCREEN_X 640
#define SCREEN_Y 480

#define SNAKE_WIDTH 8


// Pointer and variable declaration
XGpio rgb, push, adr, sws, vsync, leds, wea;

// Variable global to comunicate both state machines.
// It should be a queue.
int run;
int x_food;
int y_food;
int speed = 1;
int eaten = 0;
int received_speed=1;


//#define TRUE 1
//#define FALSE 0
	
void init_gpios() {
// Check out your own XPAR ID symbol name declared in xparameters.h
// The format is always XPAR_<NAME_IN_VIVADO>_DEVICE_ID


	XGpio_Initialize(&push, XPAR_BUTTONS_DEVICE_ID);
	XGpio_SetDataDirection(&push, 1, 0xffffffff); //input

	XGpio_Initialize(&sws, XPAR_SWS_DEVICE_ID);
	XGpio_SetDataDirection(&sws, 1, 0xffffffff); //input

	XGpio_Initialize(&vsync, XPAR_V_SYNC_DEVICE_ID);
	XGpio_SetDataDirection(&vsync, 1, 0xffffffff); //input

	XGpio_Initialize(&adr, XPAR_ADDRESS_DEVICE_ID);
	XGpio_SetDataDirection(&adr, 1, 0x00000000); //output

	XGpio_Initialize(&rgb, XPAR_COLOR_DEVICE_ID);
	XGpio_SetDataDirection(&rgb, 1, 0x00000000); //output	
	
	XGpio_Initialize(&leds, XPAR_LEDS_DEVICE_ID);
	XGpio_SetDataDirection(&leds, 1, 0x00000000); //output

	XGpio_Initialize(&wea, XPAR_WRITE_ENABLE_DEVICE_ID);
	XGpio_SetDataDirection(&wea, 1, 0x00000000); //output

}



/* Routine that writes the memory cells to represent a square */
void write_square(int xad, int yad, int color, int Delta_x, int Delta_y) {
	volatile int i, j;
	volatile int add;
	int xaux=xad;
	int yaux=yad;

	
	add = ((yad >> 1 )<< 9) | (xad >> 1);

	for(j=0; j<Delta_y; j++) {
		for(i=0; i<Delta_x; i++) {
			XGpio_DiscreteWrite(&wea, 1, 0);
			XGpio_DiscreteWrite(&adr, 1, add);
			XGpio_DiscreteWrite(&rgb, 1, color);
			XGpio_DiscreteWrite(&wea, 1, 1);

			xaux = (xaux >= SCREEN_X) ? 0 : xaux + 2;
			add = ((yaux >> 1 )<< 9) | (xaux >> 1);
			//add++;
			//add = (add >= 640) ? 0 : add+1;
		}
		yaux = (yaux >= SCREEN_Y) ? 0 : yaux + 2;
		//yad = yad +2;
		xaux=xad;
		add = ((yaux >> 1 )<< 9) | (xaux >> 1);
	}
}



/* Routine that checks input sequence */
void sequence_state_machine(void) {
	int state = 0;
	int btn, code;

	while(1)
	{
		switch (state)
		{
			case 0:
				run = FALSE;
				btn = XGpio_DiscreteRead(&push, 1);
				if (btn & 1)
				{
					code = XGpio_DiscreteRead(&sws, 1);
					if (code == 1)
					{
						state = 1;
						XGpio_DiscreteWrite(&leds, 1, 1);
						while(XGpio_DiscreteRead(&push, 1));
					}
				}
				break;
			case 1:
				btn = XGpio_DiscreteRead(&push, 1);
				if (btn & 1)
				{
						code = XGpio_DiscreteRead(&sws, 1);
						if (code == 3)
							{
								state = 2;
								XGpio_DiscreteWrite(&leds, 1, 3);
								while(XGpio_DiscreteRead(&push, 1));
							}
						else
						{
							state = 0;
							XGpio_DiscreteWrite(&leds, 1, 0);
						}
				}
				break;
			case 2:
				btn = XGpio_DiscreteRead(&push, 1);
				if (btn & 1)
				{
						code = XGpio_DiscreteRead(&sws, 1);
						if (code == 7)
							{
								state = 3;
								XGpio_DiscreteWrite(&leds, 1, 7);
								run = TRUE;
								while(XGpio_DiscreteRead(&push, 1));
							}
						else
						{
							state = 0;
							XGpio_DiscreteWrite(&leds, 1, 0);
						}
				}
				break;
			case 3:
				btn = XGpio_DiscreteRead(&push, 1);
				if (btn & 1)
				{
						code = XGpio_DiscreteRead(&sws, 1);
						if (code == 1)
							{
								state = 4;
								XGpio_DiscreteWrite(&leds, 1, 3);
								while(XGpio_DiscreteRead(&push, 1));
							}
						else
						{
							state = 3;
							XGpio_DiscreteWrite(&leds, 1, 7);
						}
				}
				break;
			case 4:
				btn = XGpio_DiscreteRead(&push, 1);
				if (btn & 1)
				{
						code = XGpio_DiscreteRead(&sws, 1);
						if (code == 3)
							{
								state = 5;
								XGpio_DiscreteWrite(&leds, 1, 1);
								while(XGpio_DiscreteRead(&push, 1));
							}
						else
						{
							state = 3;
							XGpio_DiscreteWrite(&leds, 1, 7);
						}
				}
				break;
			case 5:
				btn = XGpio_DiscreteRead(&push, 1);
				if (btn & 1)
				{
						code = XGpio_DiscreteRead(&sws, 1);
						if (code == 7)
							{
								state = 0;
								XGpio_DiscreteWrite(&leds, 1, 0);
								run = FALSE;
								while(XGpio_DiscreteRead(&push, 1));
							}
						else
						{
							state = 3;
							XGpio_DiscreteWrite(&leds, 1, 7);
						}
				}
				break;
		}
	}
	vTaskDelete(NULL);
}


void speed_read_state_machine(void) {
	int state = 0;

	while(1) {
		//XGpio_DiscreteRead(&sws, 1);
		switch (state) {
			case 0:
				//speed=1;
				if(XGpio_DiscreteRead(&sws, 1) & 8) {
					state = 1;
					new_speed = 3;
					tx_enable = 1;
				}
				break;
			case 1:
				//speed=3;
				if(!(XGpio_DiscreteRead(&sws, 1) & 8)) {
					state=0;
					new_speed = 1;
					tx_enable = 1;
				}
				break;
		}
	}
	vTaskDelete(NULL);
}


void clean_screen(int screen_colour) {
	write_square(0, 0, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
	write_square(SCREEN_X/4, 0, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
	write_square(0, SCREEN_Y/4, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
	write_square(SCREEN_X/4, SCREEN_Y/4, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);

	write_square(SCREEN_X/2, 0, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
	write_square(3*SCREEN_X/4, 0, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
	write_square(SCREEN_X/2, SCREEN_Y/4, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
	write_square(3*SCREEN_X/4, SCREEN_Y/4, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);

	write_square(0, SCREEN_Y/2, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
	write_square(SCREEN_X/4, SCREEN_Y/2, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
	write_square(0, 3*SCREEN_Y/4, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
	write_square(SCREEN_X/4, 3*SCREEN_Y/4, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);

	write_square(SCREEN_X/2, SCREEN_Y/2, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
	write_square(3*SCREEN_X/4, SCREEN_Y/2, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
	write_square(SCREEN_X/2, 3*SCREEN_Y/4, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
	write_square(3*SCREEN_X/4, 3*SCREEN_Y/4, screen_colour, SCREEN_X/8, SCREEN_Y/8);
	vTaskDelay(5);
}


void initial_screen(void) {
	clean_screen(7);
	eaten=0;
	
	int X_quad = 5;
	int Y_quad = 5;
	int offsetX = 50;
	int offsetY = 160;
	
	//S
	write_square(offsetX + 6*X_quad, offsetY + 6*Y_quad, SQUARE, X_quad*8, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 6*X_quad, offsetY + 14*Y_quad, SQUARE, X_quad*8, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 6*X_quad, offsetY + 22*Y_quad, SQUARE, X_quad*8, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 6*X_quad, offsetY + 10*Y_quad, SQUARE, X_quad*2, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 18*X_quad, offsetY + 18*Y_quad, SQUARE, X_quad*2, Y_quad*2);
	vTaskDelay(5);
	
	//N
	write_square(offsetX + 26*X_quad, offsetY + 6*Y_quad, SQUARE, X_quad*2, Y_quad*10);
	vTaskDelay(5);
	write_square(offsetX + 30*X_quad, offsetY + 6*Y_quad, SQUARE, X_quad*2, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 32*X_quad, offsetY + 10*Y_quad, SQUARE, X_quad*2, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 34*X_quad, offsetY + 14*Y_quad, SQUARE, X_quad*2, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 36*X_quad, offsetY + 18*Y_quad, SQUARE, X_quad*2, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 38*X_quad, offsetY + 22*Y_quad, SQUARE, X_quad*2, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 42*X_quad, offsetY + 6*Y_quad, SQUARE, X_quad*2, Y_quad*10);
	vTaskDelay(5);
	
	//A
	write_square(offsetX + 50*X_quad, offsetY + 6*Y_quad, SQUARE, X_quad*2, Y_quad*10);
	vTaskDelay(5);
	write_square(offsetX + 62*X_quad, offsetY + 6*Y_quad, SQUARE, X_quad*2, Y_quad*10);
	vTaskDelay(5);
	write_square(offsetX + 54*X_quad, offsetY + 6*Y_quad, SQUARE, X_quad*4, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 54*X_quad, offsetY + 14*Y_quad, SQUARE, X_quad*4, Y_quad*2);
	vTaskDelay(5);

	//K
	write_square(offsetX + 70*X_quad, offsetY + 6*Y_quad, SQUARE, X_quad*2, Y_quad*10);
	vTaskDelay(5);
	write_square(offsetX + 74*X_quad, offsetY + 14*Y_quad, SQUARE, X_quad*3, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 78*X_quad, offsetY + 10*Y_quad, SQUARE, X_quad*2, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 78*X_quad, offsetY + 18*Y_quad, SQUARE, X_quad*2, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 80*X_quad, offsetY + 6*Y_quad, SQUARE, X_quad*2, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 80*X_quad, offsetY + 22*Y_quad, SQUARE, X_quad*2, Y_quad*2);
	vTaskDelay(5);
	
	//E
	write_square(offsetX + 88*X_quad, offsetY + 6*Y_quad, SQUARE, X_quad*2, Y_quad*10);
	vTaskDelay(5);
	write_square(offsetX + 92*X_quad, offsetY + 6*Y_quad, SQUARE, X_quad*4, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 92*X_quad, offsetY + 14*Y_quad, SQUARE, X_quad*3, Y_quad*2);
	vTaskDelay(5);
	write_square(offsetX + 92*X_quad, offsetY + 22*Y_quad, SQUARE, X_quad*4, Y_quad*2);
	vTaskDelay(5);
}

void write_start(void){
	int X_quad = 5;
	int Y_quad = 5;
	int offsetX = 120;
	int offsetY = 230;
	//START:

	//S
	write_square(offsetX + 21*X_quad, offsetY + 25*Y_quad, SQUARE, X_quad*3, Y_quad*1);
	write_square(offsetX + 21*X_quad, offsetY + 29*Y_quad, SQUARE, X_quad*3, Y_quad*1);
	write_square(offsetX + 21*X_quad, offsetY + 33*Y_quad, SQUARE, X_quad*3, Y_quad*1);
	write_square(offsetX + 21*X_quad, offsetY + 27*Y_quad, SQUARE, X_quad*1, Y_quad*1);
	write_square(offsetX + 25*X_quad, offsetY + 31*Y_quad, SQUARE, X_quad*1, Y_quad*1);
	//T
	write_square(offsetX + 29*X_quad, offsetY + 25*Y_quad, SQUARE, X_quad*3, Y_quad*1);
	write_square(offsetX + 31*X_quad, offsetY + 27*Y_quad, SQUARE, X_quad*1, Y_quad*4);
	//A
	write_square(offsetX + 37*X_quad, offsetY + 25*Y_quad, SQUARE, X_quad*1, Y_quad*5);
	write_square(offsetX + 41*X_quad, offsetY + 25*Y_quad, SQUARE, X_quad*1, Y_quad*5);
	write_square(offsetX + 39*X_quad, offsetY + 25*Y_quad, SQUARE, X_quad*1, Y_quad*1);
	write_square(offsetX + 39*X_quad, offsetY + 29*Y_quad, SQUARE, X_quad*1, Y_quad*1);
	//R
	write_square(offsetX + 45*X_quad, offsetY + 25*Y_quad, SQUARE, X_quad*1, Y_quad*5);
	write_square(offsetX + 49*X_quad, offsetY + 25*Y_quad, SQUARE, X_quad*1, Y_quad*3);
	write_square(offsetX + 47*X_quad, offsetY + 25*Y_quad, SQUARE, X_quad*1, Y_quad*1);
	write_square(offsetX + 47*X_quad, offsetY + 29*Y_quad, SQUARE, X_quad*1, Y_quad*2);
	write_square(offsetX + 49*X_quad, offsetY + 33*Y_quad, SQUARE, X_quad*1, Y_quad*1);
	//T
	write_square(offsetX + 53*X_quad, offsetY + 25*Y_quad, SQUARE, X_quad*3, Y_quad*1);
	write_square(offsetX + 55*X_quad, offsetY + 27*Y_quad, SQUARE, X_quad*1, Y_quad*4);
}

void clean_start(void){
	int X_quad = 5;
	int Y_quad = 5;
	int offsetX = 120;
	int offsetY = 230;
	//S
	write_square(offsetX + 21*X_quad, offsetY + 25*Y_quad, 7, X_quad*3, Y_quad*1);
	write_square(offsetX + 21*X_quad, offsetY + 29*Y_quad, 7, X_quad*3, Y_quad*1);
	write_square(offsetX + 21*X_quad, offsetY + 33*Y_quad, 7, X_quad*3, Y_quad*1);
	write_square(offsetX + 21*X_quad, offsetY + 27*Y_quad, 7, X_quad*1, Y_quad*1);
	write_square(offsetX + 25*X_quad, offsetY + 31*Y_quad, 7, X_quad*1, Y_quad*1);
	//T
	write_square(offsetX + 29*X_quad, offsetY + 25*Y_quad, 7, X_quad*3, Y_quad*1);
	write_square(offsetX + 31*X_quad, offsetY + 27*Y_quad, 7, X_quad*1, Y_quad*4);
	//A
	write_square(offsetX + 37*X_quad, offsetY + 25*Y_quad, 7, X_quad*1, Y_quad*5);
	write_square(offsetX + 41*X_quad, offsetY + 25*Y_quad, 7, X_quad*1, Y_quad*5);
	write_square(offsetX + 39*X_quad, offsetY + 25*Y_quad, 7, X_quad*1, Y_quad*1);
	write_square(offsetX + 39*X_quad, offsetY + 29*Y_quad, 7, X_quad*1, Y_quad*1);
	//R
	write_square(offsetX + 45*X_quad, offsetY + 25*Y_quad, 7, X_quad*1, Y_quad*5);
	write_square(offsetX + 49*X_quad, offsetY + 25*Y_quad, 7, X_quad*1, Y_quad*3);
	write_square(offsetX + 47*X_quad, offsetY + 25*Y_quad, 7, X_quad*1, Y_quad*1);
	write_square(offsetX + 47*X_quad, offsetY + 29*Y_quad, 7, X_quad*1, Y_quad*2);
	write_square(offsetX + 49*X_quad, offsetY + 33*Y_quad, 7, X_quad*1, Y_quad*1);
	//T
	write_square(offsetX + 53*X_quad, offsetY + 25*Y_quad, 7, X_quad*3, Y_quad*1);
	write_square(offsetX + 55*X_quad, offsetY + 27*Y_quad, 7, X_quad*1, Y_quad*4);
}

/* Software State Machine */
void control_state_machine(void) {
	int state = 0;
	int v_sync;
	int x_new[N_max] = {};
	int y_new[N_max] = {};
	int y_old[N_max] = {};
	int x_old[N_max] = {};
	int Lx[N_max]={};
	int Ly[N_max]={};
	int orientation[N_max]={}; //el numero de mes a l'esquerra es el rectangle

	Lx[0] = 128;
	Ly[0] = SNAKE_WIDTH;
	orientation[0] = RIGHT; //el numero de mes a l'esquerra es el rectangle
	int N_current = 1;
	int btn;
	
	while(1) {
		speed = received_speed;
		//xil_printf("-x- %d --\r\n", received_speed);
		switch (state) {
			case 0:
				//INTRO SCREEN:
				//xil_printf("-x- 123 --\r\n");
				initial_screen();
				//xil_printf("-x- 13 --\r\n");
				while(!run){
					write_start();
					vTaskDelay(50);
					clean_start();
					vTaskDelay(50);
				}
				

				// ACTIVATED IF RUN = TRUE
				if(run) {
					//DELETE INTRO SCREEN
					clean_screen(BACKGROUND);
					state = 1;
					write_square(x_food, y_food, FOOD, LX_FOOD, LY_FOOD);
				}
				break;

			case 1:
				state = 2;
				int i =0;

				i=0;//head
				write_square(x_old[i], y_old[i], BACKGROUND, Lx[i], Ly[i]);
				write_square(x_new[i], y_new[i], SQUARE, Lx[i], Ly[i]);
				x_old[i] = x_new[i];
				y_old[i] = y_new[i];
				switch (orientation[i]){ //Update addresses for
						case RIGHT:
							if(N_current==1){
								x_new[i] = (x_new[i] >= SCREEN_X) ? 0 : x_new[i] + 2*speed;
							}
						break;

						case LEFT:
							x_new[i] = (x_new[i] <= 0) ? SCREEN_X : x_new[i] - 2*speed;
						break;

						case UP:
							y_new[i] = (y_new[i] <= 0) ? SCREEN_Y : y_new[i] - 2*speed;
						break;

						case DOWN:
							if(N_current==1){
								y_new[i] = (y_new[i] >= SCREEN_Y) ? 0 : y_new[i] + 2*speed;
							}
						break;
				}

				if (N_current>2){
					for(i=1; i<N_current-2; i++){
						write_square(x_new[i], y_new[i], SQUARE, Lx[i], Ly[i]);
					}
				}

				if (N_current > 1) {//tail
					i = N_current - 1;
					write_square(x_old[i], y_old[i], BACKGROUND, Lx[i], Ly[i]);
					write_square(x_new[i], y_new[i], SQUARE, Lx[i], Ly[i]);
					x_old[i] = x_new[i];
					y_old[i] = y_new[i];

					//Update addresses for
					switch (orientation[i]){
						case RIGHT:
							x_new[i] = (x_new[i] >= SCREEN_X) ? 0 : x_new[i] + 2*speed;
						break;

						//case LEFT:
							//x_new[i] = (x_new[i] <= 0) ? 640 : x_new[i]-2;
						//break;

						//case UP:
							//y_new[i] = (y_new[i] <= 0) ? 480 : y_new[i]-2;
						//break;

						case DOWN:
							y_new[i] = (y_new[i] >= SCREEN_Y) ? 0 : y_new[i] + 2*speed;
						break;
					}

					if(orientation[0] == RIGHT || orientation[0] == LEFT){ //Update length of the head (first rectangle increased)
						Lx[0] += 1*speed;
					} else {
						Ly[0] += 1*speed;
					}
					if(orientation[N_current-1] == RIGHT || orientation[N_current-1] == LEFT){ //Update length of the tail (last rectangle decreased)
						Lx[N_current-1] -= 1*speed;
						write_square(x_old[N_current-1], y_old[N_current-1], BACKGROUND, Lx[i]+1*speed, Ly[i]);//Borrem perquè si actualitzem Lx abans de borrar, el final de la cua sempre queda escrit.
						write_square(x_new[N_current-1], y_new[N_current-1], SQUARE, Lx[N_current-1], Ly[N_current-1]);
					} else {
						Ly[N_current-1] -= 1*speed;
						write_square(x_old[N_current-1], y_old[N_current-1], BACKGROUND, Lx[i], Ly[i]+1*speed);
						write_square(x_new[N_current-1], y_new[N_current-1], SQUARE, Lx[N_current-1], Ly[N_current-1]);
					}
					if(Lx[N_current-1] <= 0 || Ly[N_current-1] <= 0){
						if(Lx[N_current-1] == 0){
							write_square(x_old[N_current-1], y_old[N_current-1], BACKGROUND, Lx[N_current-1]+1*speed, Ly[N_current-1]);//Borrar l'últim rastre de la cua.
						} else{
							write_square(x_old[N_current-1], y_old[N_current-1], BACKGROUND, Lx[N_current-1], Ly[N_current-1]+1*speed);
						}
						orientation[N_current-1] = 0;
						Lx[N_current-1] = 0;
						Ly[N_current-1] = 0;
						x_new[N_current-1] = 0;
						x_old[N_current-1] = 0;
						y_new[N_current-1] = 0;
						y_old[N_current-1] = 0;
						N_current--;
					}
				}

				break;


			case 2: //We analyze which button has been pressed
				state = 3;
				btn = XGpio_DiscreteRead(&push, 1);

				if(N_current < N_max){ //If we have space for more rectangles to be drawn
					write_square(x_new[0], y_new[0], SQUARE, Lx[0], Ly[0]);

					if ((btn & RIGHT) & (orientation[0]!=RIGHT) & (orientation[0]!=LEFT)){ //Si el boto premut fa canviar de direccio l'ultim rectangle dibuixat
						for(int j=N_max-1;j>0;j--){
							orientation[j]=orientation[j-1];
							Lx[j]=Lx[j-1];
							Ly[j]=Ly[j-1];
							x_new[j]=x_new[j-1];
							x_old[j]=x_old[j-1];
							y_new[j]=y_new[j-1];
							y_old[j]=y_old[j-1];
						}
						orientation[0] = RIGHT;
						N_current++;
						Lx[0] = 0;
						Ly[0] = 8; //pq els passos son multiples de 2
						if(orientation[1]==DOWN){
							//Lx[N_current-1]-=2;
							x_new[0] = x_new[1];// + 2*Lx[1] - 2*Lx[0]; //posicio del rectangle nou
							x_old[0] = x_new[0];
							y_new[0] = (y_new[1] + 2*Ly[1]) % SCREEN_Y;
							y_old[0] = y_new[0];
						}
						if(orientation[1]==UP){
							//Lx[N_current-1]-=2;
							x_new[0] = x_new[1]; //posicio del rectangle nou
							x_old[0] = x_new[0];
							y_new[0] = (y_new[1] - 2*Ly[0]) % SCREEN_Y;
							y_old[0] = y_new[0];
						}
					}

					else if((btn & LEFT) && (orientation[0]!=LEFT) && (orientation[0]!=RIGHT)){
						for(int j=N_max-1;j>0;j--){
							orientation[j]=orientation[j-1];
							Lx[j]=Lx[j-1];
							Ly[j]=Ly[j-1];
							x_new[j]=x_new[j-1];
							x_old[j]=x_old[j-1];
							y_new[j]=y_new[j-1];
							y_old[j]=y_old[j-1];
						}
						orientation[0] = LEFT;
						N_current++;
						Lx[0] = 0;
						Ly[0] = 8; //pq els passos son multiples de 2
						if(orientation[1]==DOWN){
							//Lx[N_current-1]-=2;
							x_new[0] = x_new[1] - 2*Lx[0]; //posicio del rectangle nou
							x_old[0] = x_new[0];
							y_new[0] = (y_new[1] + 2*Ly[1] - 2*Ly[0]) % SCREEN_Y;
							y_old[0] = y_new[0];
						}
						if(orientation[1]==UP){
							//Lx[N_current-1]-=2;
							x_new[0] = x_new[1] - 2*Lx[0]; //posicio del rectangle nou
							x_old[0] = x_new[0];
							y_new[0] = y_new[1];
							y_old[0] = y_new[0];
						}
					} else if((btn & UP) && (orientation[0]!=UP) && (orientation[0]!=DOWN)){
						for(int j=N_max-1;j>0;j--){
							orientation[j]=orientation[j-1];
							Lx[j]=Lx[j-1];
							Ly[j]=Ly[j-1];
							x_new[j]=x_new[j-1];
							x_old[j]=x_old[j-1];
							y_new[j]=y_new[j-1];
							y_old[j]=y_old[j-1];
						}
						orientation[0] = UP;
						N_current++;
						Lx[0] = 8;
						Ly[0] = 0; //pq els passos son multiples de 2
						if(orientation[1]==LEFT){
							//Lx[N_current-1]-=2;
							x_new[0] = x_new[1]; //posicio del rectangle nou
							x_old[0] = x_new[0];
							y_new[0] = y_new[1] - 2*Ly[0];
							y_old[0] = y_new[0];
						}
						if(orientation[1]==RIGHT){
							//Lx[N_current-1]-=2;
							x_new[0] = (x_new[1] + 2*Lx[1] - 2*Lx[0]) % SCREEN_X; //posicio del rectangle nou
							x_old[0] = x_new[0];
							y_new[0] = y_new[1] - 2*Ly[0];
							y_old[0] = y_new[0];
						}
					} else if((btn & DOWN) && (orientation[0]!=DOWN) && (orientation[0]!=UP)){
						for(int j=N_max-1;j>0;j--){
							orientation[j]=orientation[j-1];
							Lx[j]=Lx[j-1];
							Ly[j]=Ly[j-1];
							x_new[j]=x_new[j-1];
							x_old[j]=x_old[j-1];
							y_new[j]=y_new[j-1];
							y_old[j]=y_old[j-1];
						}
						N_current++;
						orientation[0] = DOWN;
						Lx[0] = 8;
						Ly[0] = 0; //pq els passos son multiples de 2
						if(orientation[1]==RIGHT){
							//Lx[N_current-1]-=2;
							x_new[0] = (x_new[1] + 2*Lx[1] - 2*Lx[0]) % SCREEN_X; //posicio del rectangle nou
							x_old[0] = x_new[0];
							y_new[0] = (y_new[1] + 2*Ly[1]) % SCREEN_Y;
							y_old[0] = y_new[0];
						}
						if(orientation[1]==LEFT){
							//Lx[N_current-1]-=2;
							x_new[0] = x_new[1]; //posicio del rectangle nou
							x_old[0] = x_new[0];
							y_new[0] = (y_new[1] + 2*Ly[1]) % SCREEN_Y;
							y_old[0] = y_new[0];
						}
					}
				}
				break;


			case 3:
				state = 4;
				switch(orientation[0]) { // Revisar els 2
					case RIGHT:

						if(((x_new[0] + 2*Lx[0]) % SCREEN_X >= x_food) && ((x_new[0] + 2*Lx[0]) % SCREEN_X <= x_food + 2*LX_FOOD) && ((y_food >= y_new[0] && y_food <= y_new[0] + 2*Ly[0]) || (y_food + 2*LY_FOOD >= y_new[0] && y_food + 2*LY_FOOD <= y_new[0] + 2*Ly[0]))) {
							write_square(x_food, y_food, BACKGROUND, LX_FOOD, LY_FOOD);
							eaten = 1;
							Lx[0] += LX_FOOD;
						}
						break;

					case LEFT:
						if((x_new[0] >= x_food) && (x_new[0] <= x_food + 2*LX_FOOD) && ((y_food >= y_new[0] && y_food <= y_new[0] + 2*Ly[0]) || (y_food + 2*LY_FOOD >= y_new[0] && y_food + 2*LY_FOOD <= y_new[0] + 2*Ly[0]))) {
							write_square(x_food, y_food, BACKGROUND, LX_FOOD, LY_FOOD);
							eaten = 1;
							Lx[0] += LX_FOOD;
							x_new[0] -= LX_FOOD*2;
						}
						break;

					case UP:
						if((y_new[0] >= y_food) && (y_new[0] <= y_food + 2*LY_FOOD) && ((x_food >= x_new[0] && x_food <= x_new[0] + 2*Lx[0]) || (x_food + 2*LX_FOOD >= x_new[0] && x_food + 2*LX_FOOD <= x_new[0] + 2*Lx[0]))) {
							write_square(x_food, y_food, BACKGROUND, LX_FOOD, LY_FOOD);
							eaten = 1;
							Ly[0] += LY_FOOD;
							y_new[0] -= LY_FOOD*2;
						}
						break;

					case DOWN:
						if(((y_new[0] + 2*Ly[0]) % SCREEN_Y >= y_food) && ((y_new[0] + 2*Ly[0]) % SCREEN_Y <= y_food + 2*LY_FOOD) && ((x_food >= x_new[0] && x_food <= x_new[0] + 2*Lx[0]) || (x_food + 2*LX_FOOD >= x_new[0] && x_food + 2*LX_FOOD <=x_new[0] + 2*Lx[0]))) {
							write_square(x_food, y_food, BACKGROUND, LX_FOOD, LY_FOOD);
							eaten = 1;
							Ly[0] += LY_FOOD;
						}
						break;
				}
				break;


			case 4:
				/*v_sync = XGpio_DiscreteRead(&vsync, 1);
				if (v_sync) state = 0;*/
				vTaskDelay(5);
				if(run!=1){
					state = 0;
					
					//reset snake
					/*x_new[N_max] = {};
					y_new[N_max] = {};
					y_old[N_max] = {};
					x_old[N_max] = {};
					Lx[N_max]={};
					Ly[N_max]={};
					orientation[N_max]={};*/
					N_current = 1;
					Lx[0] = 128;
					Ly[0] = SNAKE_WIDTH;
					orientation[0] = RIGHT;
					x_new[0] = 0;
					x_old[0] = 0;
					y_new[0] = 0;
					y_old[0] = 0;
				}
				else {
					state=1;
				}
				break;
		}
	}
	vTaskDelete(NULL);

}

void control_menjar(void){

	//x_food = rand() % (SCREEN_X - LX_FOOD + 1); //Screen size - Square size + 1
	//y_food = rand() % (SCREEN_Y - LY_FOOD + 1); //idem
	x_food = rand() % (SCREEN_X - LX_FOOD + 1);
	y_food = rand() % (SCREEN_Y - LY_FOOD + 1);
	if(run) write_square(x_food, y_food, FOOD, LX_FOOD, LY_FOOD);

	while(1) {
		if(run == 1) {
			if(eaten) {
					x_food = rand() % (SCREEN_X - LX_FOOD + 1); //mida pantalla - mida quadrat +1
					y_food = rand() % (SCREEN_Y - LY_FOOD + 1); //idem
					write_square(x_food, y_food, FOOD, LX_FOOD, LY_FOOD);
					eaten = 0;
				}
			else {
				write_square(x_food, y_food, FOOD, LX_FOOD, LY_FOOD);
			}
		}
		vTaskDelay(5);
	}

}



int main (void) {

    xil_printf("-- Start of the Program --\r\n");
	init_gpios();
	//srand(time(NULL)); // Initialization, should only be called once.


	xTaskCreate ((void(*)(void*))sequence_state_machine, "task1", 200, NULL, 2, NULL);
	xTaskCreate ((void(*)(void*))control_state_machine, "task2", 200, NULL, 2, NULL);
	xTaskCreate ((void(*)(void*))control_menjar, "task3", 200, NULL, 2, NULL);
	xTaskCreate ((void(*)(void*))speed_read_state_machine, "task4", 200, NULL, 2, NULL);

	//xTaskCreate ((void(*)(void*))main_connection, "task4", 200, NULL, 1, NULL);

	main_connection();

	vTaskStartScheduler();


    while(1); // infinite loop
    return 0;
}
		  
