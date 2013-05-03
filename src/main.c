/*
	Main Thread

	This program is used to control a Mentor Robot arm.

	Github: https://github.com/LewisJared/MentorRevival
*/

#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#include "joystick.h"
#include "motor.h"


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
   * Activates the UART driver 2, PD5(TX) and PD6(RX) are routed to USART2.
   */
  palSetPadMode(GPIOD, 5, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOD, 6, PAL_MODE_ALTERNATE(7));

  //sdObjectInit(&SD2, NULL, NULL);
  sdStart(&SD2,NULL);

  joy_init();
  motor_init();

  joypos_t joystickData[NUM_JOYSTICKS];
  int i;

  //Event loop simply updates the current positions
  while (TRUE) {
    chThdSleepMilliseconds(200);

    // Get the latest joystick Vals
	joy_getValues(joystickData);

    //Now print the data to the serial driver
    for (i = 0; i < NUM_JOYSTICKS; i++)
    {
    	chprintf((BaseSequentialStream*)&SD2, "Val %d:%d %d ", i, joystickData[i].pos, joystickData[i].dir);
    	motor_setDir(i, joystickData[i].dir);
    	motor_setSpeed(i, joystickData[i].pos);
    }
    chprintf((BaseSequentialStream*)&SD2, "\r\n");
  }
}
