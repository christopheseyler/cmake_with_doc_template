
/** The conditional compilation allows to specify if the app is considered as a SLAVE or a MASTER
 * core app
 *
 *  The TARGET_CPU_${TARGET_CPU} is defined as follows :
 *  - On Windows static library
 *  - For Cross compiling specific library
 *  The only case where it isn't defined is for Unit Testing
 *  In that case all includes are enabled but COM includes are preferred as common.
 */

#if defined(TARGET_CPU_COM)
#include "app/app_sync_init/app_sync_init_master/app_sync_init_master.h"
#include "event_log/event_log_com.h"
#include "fatal_error/fatal_error_master/fatal_error_master.h"
#include "hardware_initialization_com/hardware_initialization_com.h"
#define MASTER_CORE
#elif defined(TARGET_CPU_RAS)
#include "app/app_sync_init/app_sync_init_slave/app_sync_init_slave.h"
#include "event_log/event_log_ras.h"
#include "fatal_error/fatal_error_slave/fatal_error_slave.h"
#include "hardware_initialization_ras/hardware_initialization_ras.h"
#define SLAVE_CORE
#elif defined(TARGET_CPU_SSM)
#include "app/app_sync_init/app_sync_init_slave/app_sync_init_slave.h"
#include "event_log/event_log_ssm.h"
#include "fatal_error/fatal_error_slave/fatal_error_slave.h"
#include "hardware_initialization_ssm/hardware_initialization_ssm.h"
#define SLAVE_CORE
#endif

#if !defined(CROSS_COMPILING) && !defined(TARGET_CPU_COM) && !defined(TARGET_CPU_RAS) &&           \
    !defined(TARGET_CPU_SSM)
// represents Unit tests case
#if defined(MASTER_CORE)
#include "app/app_sync_init/app_sync_init_master/app_sync_init_master.h"
#include "fatal_error/fatal_error_master/fatal_error_master.h"
#elif defined(SLAVE_CORE)
#include "app/app_sync_init/app_sync_init_slave/app_sync_init_slave.h"
#include "fatal_error/fatal_error_slave/fatal_error_slave.h"
#endif
#include "event_log/event_log_com.h"
#include "hardware_initialization_com/hardware_initialization_com.h"
#endif

#include "app/app_boot_status/app_boot_status.h"
#include "app/app_fsm/app_fsm.h"
#include "app/app_task_sequencer/app_task_sequencer.h"
#include "dpc_hal/interrupt/interrupt.h"
#include "dpc_hal/ltc/ltc.h"
#include "dpc_hal/watchdog/watchdog.h"

AppFsmHandler app_fsm_state = app_fsm_boot_check;

static uint16_t app_sync_start_monitoring_time = 0;

static inline void app_fsm_boot_check_entry(void)
{
   hardware_initialization_initialize();

   event_log_initialize();
}

void app_fsm_boot_check(void)
{
   app_fsm_boot_check_entry();

   app_boot_status_check();

   app_fsm_state = app_fsm_initializers;
}

void app_fsm_initializers(void)
{
   WATCHDOG_SINGLE_SOURCE_CLEAR_COUNTER();

   app_task_sequencer_init();

#ifdef SLAVE_CORE
   app_sync_init_done();
#endif

   app_sync_start_monitoring_time = hal_ltc_read_16bits();

   app_fsm_state = app_fsm_wait_for_start;

#ifdef MASTER_CORE
   fatal_error_master_check_slave_error();
#endif
}

void app_fsm_wait_for_start(void)
{
   WATCHDOG_SINGLE_SOURCE_CLEAR_COUNTER();

   if (app_sync_init_is_ordered_to_start(app_sync_start_monitoring_time))
   {
      dpc_enable_interrupt();
      app_fsm_state = app_fsm_updaters;
   }
   else
   {
      // intentionally left blank
   }

#ifdef MASTER_CORE
   fatal_error_master_check_slave_error();
#endif
}

void app_fsm_updaters(void)
{
   WATCHDOG_SINGLE_SOURCE_CLEAR_COUNTER();

   app_task_sequencer_update();

#ifdef MASTER_CORE
   fatal_error_master_check_slave_error();
#endif
}
