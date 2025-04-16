/** @file 
 * 
 * The App Boot Status checks if the core is in a watchdog state or if an unexpected
 * reboot was encountered.
 * 
 * In case of problem in goes into the fatal error state.
 * 
 */

#pragma once

#include <stdbool.h>

/** The function checks if the core has started normally, going to fatal error state in case of any error. */
void app_boot_status_check(void);


