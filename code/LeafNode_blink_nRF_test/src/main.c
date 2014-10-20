/**************************************************************************/
/*!
    @file     main.c

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2013, K. Townsend (microBuilder.eu)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#include <stdio.h>
#include "LPC8xx.h"
#include "main.h"
#include "gpio.h"
#include "mrt.h"
#include "uart.h"
#include "spi.h"
#include "nrf24.h"
#include "nRF24L01.h"
#include "type.h"

#define CE 14
#define CSN 7

#if defined(__CODE_RED)
  #include <cr_section_macros.h>
  #include <NXP/crp.h>
  __CRP const unsigned int CRP_WORD = CRP_NO_CRP ;
#endif

#define LED_LOCATION    (2)

/* This define should be enabled if you want to      */
/* maintain an SWD/debug connection to the LPC810,   */
/* but it will prevent you from having access to the */
/* LED on the LPC810 Mini Board, which is on the     */
/* SWDIO pin (PIO0_2).                               */
// #define USE_SWD
  void SwitchMatrix_Init()
  {
      /* Enable SWM clock */
      //LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7); already done in systeminit

      /* Pin Assign 8 bit Configuration */
      /* U0_TXD */
      /* U0_RXD */
      LPC_SWM->PINASSIGN0 = 0xffff0004UL;
      /* SPI0_SCK */
      LPC_SWM->PINASSIGN3 = 0x08ffffffUL;
      /* SPI0_MOSI */
      /* SPI0_MISO */
      /* SPI0_SSEL */
      //LPC_SWM->PINASSIGN4 = 0xff070d03UL;
      LPC_SWM->PINASSIGN4 = 0xffff0d03UL;

      /* Pin Assign 1 bit Configuration */
      /* SWDIO */
      /* RESET */
      LPC_SWM->PINENABLE0 = 0xffffffbfUL;
  }

void configurePins()
{
  /* Enable SWM clock */
	//  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);  // this is already done in SystemInit()

  /* Pin Assign 8 bit Configuration */
  /* U0_TXD */
  /* U0_RXD */
  LPC_SWM->PINASSIGN0 = 0xffff0004UL;

  /* Pin Assign 1 bit Configuration */
  #if !defined(USE_SWD)
    /* Pin setup generated via Switch Matrix Tool
       ------------------------------------------------
       PIO0_5 = RESET
       PIO0_4 = U0_TXD
       PIO0_3 = GPIO            - Disables SWDCLK
       PIO0_2 = GPIO (User LED) - Disables SWDIO
       PIO0_1 = GPIO
       PIO0_0 = U0_RXD
       ------------------------------------------------
       NOTE: SWD is disabled to free GPIO pins!
       ------------------------------------------------ */
    LPC_SWM->PINENABLE0 = 0xffffffbfUL;
  #else
    /* Pin setup generated via Switch Matrix Tool
       ------------------------------------------------
       PIO0_5 = RESET
       PIO0_4 = U0_TXD
       PIO0_3 = SWDCLK
       PIO0_2 = SWDIO
       PIO0_1 = GPIO
       PIO0_0 = U0_RXD
       ------------------------------------------------
       NOTE: LED on PIO0_2 unavailable due to SWDIO!
       ------------------------------------------------ */
    LPC_SWM->PINENABLE0 = 0xffffffb3UL;   
  #endif  
}

void nrf24_ce_digitalWrite(uint8_t state){
	gpioSetValue(0, CE, state);
}

void nrf24_csn_digitalWrite(uint8_t state){
	gpioSetValue(0, CSN, state);
}

uint8_t spi_transfer(uint8_t tx, uint8_t send_eot)
{
    spiTransfer(LPC_SPI0, tx, send_eot);
    return LPC_SPI0->RXDAT;
}

uint8_t rx_data_array[16];
uint8_t tx_data_array[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
uint8_t tx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};

int main(void)
{
  /* Initialise the GPIO block */
  gpioInit();

  /* Initialise the UART0 block for printf output */
  //uart0Init(115200);
  uart0Init(9600);
  /* Configure the multi-rate timer for 1ms ticks */
  mrtInit(__SYSTEM_CLOCK/1000);

  /* Configure the switch matrix (setup pins for UART0 and GPIO) */
  //configurePins();
  spiInit(LPC_SPI0, 6, 0);
  SwitchMatrix_Init(); //uart & spi
  LPC_GPIO_PORT->DIR0 |= (1 << CSN);
  LPC_GPIO_PORT->DIR0 |= (1 << CE);
  uint8_t temp = 0;

  nrf24_init();




  /* Set the LED pin to output (1 = output, 0 = input) */
  #if !defined(USE_SWD)
    LPC_GPIO_PORT->DIR0 |= (1 << LED_LOCATION);
  #endif

    //printf("write");
    mrtDelay(500);

    nrf24_config(2,16);

	#if TX_NODE
    	nrf24_tx_address(tx_address);
    	nrf24_rx_address(rx_address);
	#else
    	nrf24_tx_address(rx_address); //backwards looking but is fine
    	nrf24_rx_address(tx_address);
	#endif

    uint8_t i = 0;
  while(1)
  {

      /* Turn LED On by setting the GPIO pin high */
      LPC_GPIO_PORT->SET0 = 1 << LED_LOCATION;
      mrtDelay(500);

      /* Turn LED Off by setting the GPIO pin low */
      LPC_GPIO_PORT->CLR0 = 1 << LED_LOCATION;
      mrtDelay(500);

	  /*
      printf("Send\r\n");
      for(i=0; i<16; i++){
    	  printf("%d: %d\n\r", i, tx_data_array[i]);
      }
	   */
      nrf24_send(tx_data_array);
      while(nrf24_isSending());
      nrf24_powerUpRx();
      //done transmitting, set back to rx mode

      for(i=0; i<16; i++){
    	  tx_data_array[i]++;
      }

/*
      if(nrf24_dataReady()){
    	  printf("\n\rGot data\n\r");

          nrf24_getData(rx_data_array);
          for(i=0; i<16; i++){
          printf("%d: %d\n\r", i, rx_data_array[i]);
          }
      }
*/
  }
}

/*
      //status = nrf24_getStatus();
      //printf("Status: %d\n\r", status);
      nrf24_readRegister(RF_CH, &temp,1);
      printf("RF_CH: %d\n\r", temp);


      nrf24_readRegister(RX_PW_P0, &temp,1);
      printf("RX_PW_P0: %d\n\r", temp);

      nrf24_readRegister(RX_PW_P1, &temp,1);
      printf("RX_PW_P1: %d\n\r", temp);

      nrf24_readRegister(RX_PW_P2, &temp, 1);
      printf("RX_PW_P2: %d\n\r", temp);

      nrf24_readRegister(RX_PW_P3, &temp, 1);
      printf("RX_PW_P3: %d\n\r", temp);

      nrf24_readRegister(RX_PW_P4, &temp, 1);
      printf("RX_PW_P4: %d\n\r", temp);

      nrf24_readRegister(RX_PW_P5, &temp, 1);
      printf("RX_PW_P5: %d\n\r", temp);

      nrf24_readRegister(RF_SETUP, &temp, 1);
      printf("RF_SETUP: %d\n\r", temp);

      nrf24_readRegister(CONFIG, &temp, 1);
      printf("CONFIG: %d\n\r", temp);

      nrf24_readRegister(EN_AA, &temp, 1);
      printf("EN_AA: %d\n\r", temp);

      nrf24_readRegister(EN_RXADDR, &temp, 1);
      printf("EN_RXADDR: %d\n\r", temp);

      nrf24_readRegister(SETUP_RETR, &temp, 1);
      printf("SETUP_RETR: %d\n\r", temp);

      nrf24_readRegister(DYNPD, &temp, 1);
      printf("DYNPD: %d\n\r", temp);
*/

