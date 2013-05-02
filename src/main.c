/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#include "adcThread.h"

static const SerialConfig serConf ={
	38400,
	0,
	USART_CR2_LINEN,
	0
};

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the UART driver 2, PA2(TX) and PA3(RX) are routed to USART2.
   */
  //uartStart(&UARTD2, &uart_cfg_1);
  palSetPadMode(GPIOD, 5, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOD, 6, PAL_MODE_ALTERNATE(7));

  //sdObjectInit(&SD2, NULL, NULL);
  sdStart(&SD2,NULL);

  ADC_init();

  adcsample_t copyData[ADC_NUM_CHANNELS];
  int i;

  //Event loop simply updates the current positions
  while (TRUE) {
    chThdSleepMilliseconds(200);

    //Copy the adc values to local data
    chSysLock();

    for (i = 0; i < ADC_NUM_CHANNELS; i++)
    {
    	copyData[i] = currentPos[i];
    }
    chSysUnlock();

    //Now print the data to the serial driver
    for (i = 0; i < ADC_NUM_CHANNELS; i++)
    	chprintf((BaseSequentialStream*)&SD2, "Val %d:%d ", i, copyData[i]);
    chprintf((BaseSequentialStream*)&SD2, "\r\n");
  }
}
