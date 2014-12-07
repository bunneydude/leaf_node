/*
 * main.h
 *
 *  Created on: Oct 19, 2014
 *      Author: mabunney
 */

#ifndef MAIN_H_
#define MAIN_H_

#define SREG_NODE_ID 0
#define SREG_TX_PWR 1
#define SREG_RSSI 2
#define SREG_LQI 3
#define SREG_TEMPERATURE 4

#define SREG_HBT 8
#define SREG_SET_NODE_ID 9
#define SREG_SET_TX_PWR 10
#define DOGE_PAYLOAD 7

#define NRF_CE 14
#define NRF_CSN 7
#define NRF_IRQ 17

#define LPC_ISP 12
#define LPC_SCL 11
#define LPC_SDA 10

#define LPC_ADC 6
#define LPC_TEMP1 9
#define LPC_TEMP2 1

#define LPC_TP1 16
#define LPC_TP2 15

#define RED_CH 0
#define GREEN_CH 1
#define BLUE_CH 2

void print_nrf_regs();


#endif /* MAIN_H_ */
