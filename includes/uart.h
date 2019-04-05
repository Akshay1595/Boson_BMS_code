/*
 * uart.h
 *
 *  Created on: Mar 23, 2019
 *      Author: Akshay Godase
 */

#ifndef INCLUDES_UART_H_
#define INCLUDES_UART_H_

#define UART_RX_PIN 9 //GPIO9
#define UART_RX_PF  6   //PERIPHERAL SELECT VALUE FOR MUX

#define UART_TX_PIN 8 //GPIO8
#define UART_TX_PF  6   //PERIPHERAL SELECT VALUE FOR MUX

void uart_init(void);
void gpio_uart_init(void);
Uint8 uart_get_char(void);
void uart_xmit(int a);
void uart_string(Uint8 * msg);
void uart_receive_buffer(Uint8 *buf);
void uart_string_newline(Uint8* buf);
void my_itoa(Uint16 a,char *buf);
void reverse(char *str, int len);
void float_to_ascii( double number,Uint8 *buf);

#endif /* INCLUDES_UART_H_ */
