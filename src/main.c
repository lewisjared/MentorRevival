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
#include "events.h"

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

  //Event loop simply updates the current positions
  while (TRUE) {
    chThdSleepMilliseconds(200);
  }
}
