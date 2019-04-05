/*
 * uart.c
 *
 *  Created on: Mar 23, 2019
 *      Author: Akshay Godase
 */
#include "all_header.h"
/*
 * This function inits GPIO9 and GPIO8 as UART RX and TX pin resp.
 */
void gpio_uart_init()
{
    GPIO_SetupPinMux(UART_RX_PIN, GPIO_MUX_CPU1, UART_RX_PF);
    GPIO_SetupPinOptions(UART_RX_PIN, GPIO_INPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(UART_TX_PIN, GPIO_MUX_CPU1, UART_TX_PF);
    GPIO_SetupPinOptions(UART_TX_PIN, GPIO_OUTPUT, GPIO_ASYNC);

}

/*
*  uart_init - Test 1,SCIA  DLB, 8-bit word, baud rate 0x000F,
*                       default, 1 STOP bit, no parity
*/
void uart_init()
{
    //
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    //

    gpio_uart_init();
    SciaRegs.SCICCR.all = 0x0007;   // 1 stop bit,  No loopback
                                    // No parity,8 char bits,
                                    // async mode, idle-line protocol
    SciaRegs.SCICTL1.all = 0x0003;  // enable TX, RX, internal SCICLK,
                                    // Disable RX ERR, SLEEP, TXWAKE
    SciaRegs.SCICTL2.all = 0x0003;
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;

    //
    // SCIA at 9600 baud
    // @LSPCLK = 50 MHz (200 MHz SYSCLK) HBAUD = 0x02 and LBAUD = 0x8B.
    // @LSPCLK = 30 MHz (120 MHz SYSCLK) HBAUD = 0x01 and LBAUD = 0x86.
    //
    SciaRegs.SCIHBAUD.all = 0x0002;
    SciaRegs.SCILBAUD.all = 0x008B;

    SciaRegs.SCICTL1.all = 0x0023;  // Relinquish SCI from Reset

    //
    // scia_fifo_init - Initialize the SCI FIFO
    //
    SciaRegs.SCIFFTX.all = 0xE040;
    SciaRegs.SCIFFRX.all = 0x2044;
    SciaRegs.SCIFFCT.all = 0x0;
}

//
// uart_xmit - Transmit a character from the SCI
//
void uart_xmit(int a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0) {}
    SciaRegs.SCITXBUF.all =a;
}

//
// uart_msg - Transmit message via SCIA
//
void uart_string(Uint8 * msg)
{
    Uint8 i=0;
    while(msg[i] != '\0')
    {
        uart_xmit(msg[i]);
        i++;
    }
}

//
// uart_msg - Transmit message via SCIA but it adds newline characters needed for debugging
//
void uart_string_newline(Uint8 * msg)
{
    uart_xmit('\r');
    uart_xmit('\n');
    uart_string(msg);
}
//
//  This function receives single character
//
Uint8 uart_get_char()
{
    Uint8 rcvd_character;
    //
    // Wait for inc character
    //
    while(SciaRegs.SCIFFRX.bit.RXFFST == 0) { } // wait for empty state

    //
    // Get character
    //
     rcvd_character = SciaRegs.SCIRXBUF.all;

     return rcvd_character;
}

//
//  This function receives buffer of characters
//
void uart_receive_buffer(Uint8 *buf)
{
    Uint8 count = 0;
    Uint8 ch;
    ch = uart_get_char() ;
    while(ch!= '\n' && ch!= '\r')
    {
        buf[count++] = ch;
        ch = uart_get_char();
    }
    buf[count]=0;
}
/*
 *  Description: myitoa implementation
 */
void my_itoa(Uint16 a,char *buf)
{
    int index = 0;
    if(a == 0)
    {
        buf[index++] = '0';
    }
    while(a)
    {
        buf[index++] = (a % 10) + 48;
        a = a/10;
    }
    buf[index] = 0;
    int len = strlen(buf);
    reverse(buf,len);
}
void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
void float_to_ascii(double number,Uint8 *buf)
{
    Uint16 i_num = number;
    my_itoa(i_num,buf);
    strcat(buf,".");
    Uint16 float_num = ((Uint32)((double)number * 1000)) - (Uint32)(i_num*1000);
    char buf2[10] = {};
    my_itoa(float_num,buf2);
    strcat(buf,buf2);
}
