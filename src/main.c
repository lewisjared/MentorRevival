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

  joy_init();
  motor_init();
  evt_init();

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
    	motor_setDir(i, joystickData[i].dir);
    	motor_setSpeed(i, joystickData[i].pos);
    }
  }
}
