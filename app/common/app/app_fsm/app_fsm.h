/** @file
  
The application fsm module exposes the application states handlers.
 
 */

#pragma once

typedef void (*AppFsmHandler)(void);

/** Function Pointer to the current state handler. */
extern AppFsmHandler app_fsm_state;

/** This function updates the progress of the fsm by calling the current state handler.
 * This function requires to be called periodically to allow the fsm to progress.
 */
inline void app_fsm_update(void)
{
    app_fsm_state();
}

/** Application fsm state handler for boot check state. */
void app_fsm_boot_check(void);

/** Application fsm state handler for initializer state */
void app_fsm_initializers(void);

/** Application fsm state handler for wait for start state */
void app_fsm_wait_for_start(void);

/** Application fsm state handler for updaters state */
void app_fsm_updaters(void);


