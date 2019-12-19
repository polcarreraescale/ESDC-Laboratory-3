/*
 * design2_FreeRTOS.h
 *
 *  Created on: 12 des. 2019
 *      Author: marc.martinez.gost
 */

#ifndef SRC_DESIGN2_FREERTOS_H_
#define SRC_DESIGN2_FREERTOS_H_

void init_gpios();
void write_square(int xad, int yad, int color, int Delta_x, int Delta_y);
void sequence_state_machine(void);
void speed_read_state_machine(void);
void clean_screen(int screen_colour);
void initial_screen(void);
void control_state_machine(void);
void control_menjar(void);

#endif /* SRC_DESIGN2_FREERTOS_H_ */
