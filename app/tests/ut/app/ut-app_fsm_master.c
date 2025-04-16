#include "unit_test.h"

#include "app/app_fsm/app_fsm.h"

#include <stdbool.h>
#include <stdint.h>

#define TEST_GROUP ut_app_fsm_master

extern AppFsmHandler app_fsm_state;

static int setup_boot_check(void** ppState_v)
{
   app_fsm_state = app_fsm_boot_check;

   return 0;
}

static int setup_initializers(void** ppState_v)
{
   app_fsm_state = app_fsm_initializers;

   return 0;
}

static int setup_wait_for_start(void** ppState_v)
{
   app_fsm_state = app_fsm_wait_for_start;

   return 0;
}

static int setup_updaters(void** ppState_v)
{
   app_fsm_state = app_fsm_updaters;

   return 0;
}

/** @utdef{UT-APP-FSM-0010 | Test app_fsm_boot_check transitions to app_fsm_initializers}
    :layout: test
    :tags: su, app_fsm_master, app_fsm_boot_check
    :checks: DLT-SRS-APP-FSM-FUN-001, DLT-SRS-APP-FSM-FUN-002, DLT-SRS-APP-FSM-FUN-003

    - GIVEN the system is in the app_fsm_boot_check state
    - WHEN app_fsm_boot_check is executed
    - THEN the system transitions to app_fsm_initializers if no fatal error occurs
 @endut*/
TEST(fsm_boot_check, setup_boot_check)
{
   expect_hardware_initialization_initialize();
   expect_event_log_initialize();
   expect_app_boot_status_check();

   app_fsm_boot_check();

   assert_int_equal(app_fsm_state, app_fsm_initializers);
}

/** @utdef{UT-APP-FSM-0020 | Test app_fsm_initializers transitions to app_fsm_wait_for_start}
    :layout: test
    :tags: su, app_fsm_master, app_fsm_initializers
    :checks: DLT-SRS-APP-FSM-FUN-011

    - GIVEN the system is in the app_fsm_initializers state
    - WHEN app_fsm_initializers is executed
    - THEN the system transitions to app_fsm_wait_for_start if no fatal error occurs
 @endut*/
TEST(fsm_initializers, setup_boot_check)
{
   expect_hal_watchdog_single_source_clear_counter();
   expect_app_task_sequencer_init();
   expect_hal_ltc_read_16bits(0);
   expect_fatal_error_master_check_slave_error();

   app_fsm_initializers();

   assert_int_equal(app_fsm_state, app_fsm_wait_for_start);
}

/** @utdef{UT-APP-FSM-0030 | Test app_fsm_wait_for_start remains in the same state if not ordered to start}
    :layout: test
    :tags: su, app_fsm_master, app_fsm_wait_for_start
    :checks: DLT-SRS-APP-FSM-FUN-001-001, DLT-SRS-APP-FSM-FUN-005

    - GIVEN the system is in the app_fsm_wait_for_start state
    - WHEN app_fsm_wait_for_start is executed and start order is not received
    - THEN the system remains in the app_fsm_wait_for_start state
 @endut*/
TEST(fsm_wait_for_start_not_ordered_to_start, setup_wait_for_start)
{
   expect_hal_watchdog_single_source_clear_counter();
   expect_app_sync_init_is_ordered_to_start(0, false);
   expect_fatal_error_master_check_slave_error();

   app_fsm_wait_for_start();

   assert_int_equal(app_fsm_state, app_fsm_wait_for_start);
}

/** @utdef{UT-APP-FSM-0040 | Test app_fsm_wait_for_start transitions to app_fsm_updaters when ordered to start}
    :layout: test
    :tags: su, app_fsm_master, app_fsm_wait_for_start
    :checks: DLT-SRS-APP-FSM-FUN-001-001, DLT-SRS-APP-FSM-FUN-005

    - GIVEN the system is in the app_fsm_wait_for_start state
    - WHEN app_fsm_wait_for_start is executed and start order is received
    - THEN the system transitions to app_fsm_updaters
 @endut*/
TEST(fsm_wait_for_start_ordered_to_start, setup_wait_for_start)
{
   expect_hal_watchdog_single_source_clear_counter();
   expect_app_sync_init_is_ordered_to_start(0, true);
   expect_dpc_enable_interrupt();
   expect_fatal_error_master_check_slave_error();

   app_fsm_wait_for_start();

   assert_int_equal(app_fsm_state, app_fsm_updaters);
}

/** @utdef{UT-APP-FSM-0050 | Test app_fsm_updaters remains in the same state if no fatal error occurs}
    :layout: test
    :tags: su, app_fsm_master, app_fsm_updaters
    :checks: DLT-SRS-APP-FSM-FUN-006

    - GIVEN the system is in the app_fsm_updaters state
    - WHEN app_fsm_updaters is executed
    - THEN the system remains in the app_fsm_updaters state if no fatal error occurs
 @endut*/
TEST(fsm_updaters, setup_updaters)
{
   expect_hal_watchdog_single_source_clear_counter();
   expect_app_task_sequencer_update();
   expect_fatal_error_master_check_slave_error();

   app_fsm_updaters();

   assert_int_equal(app_fsm_state, app_fsm_updaters);
}
