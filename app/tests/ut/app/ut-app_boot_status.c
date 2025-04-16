/** @file
 * Unit tests suite for app_boot_status component.
 */

#include "unit_test.h"
#include "event_log/event_log_common.h"
#include "app/app_boot_status/app_boot_status.h"
#include "log.h"

extern uint16_t app_boot_status_global_variable;

#define TEST_GROUP ut_app_boot_status

/** Setup application boot status as a normal startup */
static int setup_app_boot_status_normal_startup(void** ppState_v)
{
   app_boot_status_global_variable = 0;

   return 0;
}

/** Setup application boot status as an unexpected reboot startup */
static int setup_app_boot_status_unexpected_reboot_startup(void** ppState_v)
{
   app_boot_status_global_variable = 0xA55Au;

   return 0;
}

/** @utdef{UT-APP-BOOT-STATUS-0010 | Normal boot sequence detection with no watchdog flag}
    :layout: test
    :tags: dlt, ut, app_boot_status
    :checks: SU-SRS-APP-00060

    - GIVEN the application boot status is not initialized
    - WHEN app_boot_status_check is called and the watchdog flag is not activated
    - THEN the global variable is initialized to 0xA55A indicating a normal boot

 @endut */
TEST(status_check_no_watchdog_flag_normal_boot, setup_app_boot_status_normal_startup)
{
   expect_hal_watchdog_is_flag_activated(false);

   app_boot_status_check();

   assert_int_equal(app_boot_status_global_variable, 0xA55Au);
}

/** @utdef{UT-APP-BOOT-STATUS-0020 | Fatal error when watchdog flag is set}
    :layout: test
    :tags: dlt, ut, app_boot_status
    :checks: SU-SRS-APP-00060, SU-SRS-APPFSM-00080

    - GIVEN the application boot status is not initialized
    - WHEN app_boot_status_check is called and the watchdog flag is activated
    - THEN the fatal_error_raise_after_watchdog function is called

 @endut */
TEST(status_check_watchdog_flag_set, setup_app_boot_status_normal_startup)
{
   expect_hal_watchdog_is_flag_activated(true);

   expect_fatal_error_raise_after_watchdog();

   expect_assert_failure(app_boot_status_check());
}

/** @utdef{UT-APP-BOOT-STATUS-0030 | Critical log emitted for unexpected reboot}
    :layout: test
    :tags: dlt, ut, app_boot_status
    :checks: SU-SRS-APP-00060, SU-SRS-APPFSM-00080

    - GIVEN the application boot status is already initialized to 0xA55A (unexpected reboot)
    - WHEN app_boot_status_check is called and the watchdog flag is not activated
    - THEN a critical log message is emitted to indicate unexpected reboot

 @endut */
TEST(status_check_no_watchdog_flag_unexpected_reboot,
     setup_app_boot_status_unexpected_reboot_startup)
{
   expect_hal_watchdog_is_flag_activated(false);
   expect_codelog_emit_n0(CODELOG_SEVERITY_CRITICAL, com_app_boot_status, 1u);

   app_boot_status_check();
}

/** @} */
