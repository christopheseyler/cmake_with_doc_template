#include <stdint.h>

/** The conditional compilation allows to specify the module id used by the fatal
 * error writer */
/**
 *  The TARGET_CPU_${TARGET_CPU} is defined as follows :
 *  - On Windows static library
 *  - For Cross compiling specific library
 *  The only case where it isn't defined is for Unit Testing
 *  In that case all includes are enabled but COM includes are preferred as common.
 */

#if defined(TARGET_CPU_COM)
#define CODELOG_MODULE com_app_boot_status
#include "com.regmap.h"
#include "fatal_error/fatal_error_master/fatal_error_master.h"
static uint16_t* const app_boot_status_global_variable_address =
    (uint16_t*)(COM_MB_EMU_APP_BOOT_STATUS_BOOT_STATUS_ADDRESS);
#elif defined(TARGET_CPU_SSM)
#define CODELOG_MODULE ssm_app_boot_status
#include "fatal_error/fatal_error_slave/fatal_error_slave.h"
#include "ssm.regmap.h"
static uint16_t* const app_boot_status_global_variable_address =
    (uint16_t*)(SSM_APP_BOOT_STATUS_BOOT_STATUS_ADDRESS);
#elif defined(TARGET_CPU_RAS)
#define CODELOG_MODULE ras_app_boot_status
#include "fatal_error/fatal_error_slave/fatal_error_slave.h"
#include "ras.regmap.h"
static uint16_t* const app_boot_status_global_variable_address =
    (uint16_t*)(RAS_APP_BOOT_STATUS_BOOT_STATUS_ADDRESS);
#endif

#ifndef CROSS_COMPILING

#if defined(TARGET_CPU_COM)
#elif defined(TARGET_CPU_RAS)
#elif defined(TARGET_CPU_SSM)
#else  // represents Unit tests case
#define CODELOG_MODULE com_app_boot_status
#include "fatal_error/fatal_error_master/fatal_error_master.h"
uint16_t               app_boot_status_global_variable;
static uint16_t* const app_boot_status_global_variable_address = &app_boot_status_global_variable;
#endif
#endif

#include "app/app_boot_status/app_boot_status.h"
#include "dpc_hal/watchdog/watchdog.h"
#include "log.h"

#define BOOT_STATUS_GLOBAL_VARIABLE_VALUE_AFTER_START 0xA55Au

void app_boot_status_check(void)
{
   if (hal_watchdog_is_flag_activated())
   {
      fatal_error_raise_after_watchdog();
   }

   if (*app_boot_status_global_variable_address == BOOT_STATUS_GLOBAL_VARIABLE_VALUE_AFTER_START)
   {
      LOG_CRITICAL(1, "Unexpected restart of Core");
   }
   else
   {
      *app_boot_status_global_variable_address = BOOT_STATUS_GLOBAL_VARIABLE_VALUE_AFTER_START;
   }
}
