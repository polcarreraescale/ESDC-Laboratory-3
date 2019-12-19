/*
 * connection.h
 *
 *  Created on: 12 des. 2019
 *      Author: marc.martinez.gost
 */

#ifndef SRC_CONNECTION_H_
#define SRC_CONNECTION_H_

void print_ip(char *msg, struct ip_addr *ip);
void print_ip_settings(struct ip_addr *ip, struct ip_addr *mask, struct ip_addr *gw);
int main_connection();
void network_init();
void tx_data(void *);
void rx_data(void *);
int main_thread();
void print_headers();
void vApplicationMallocFailedHook( void );
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName );
void vApplicationSetupHardware( void );

#endif /* SRC_CONNECTION_H_ */
