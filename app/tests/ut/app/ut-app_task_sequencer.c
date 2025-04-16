#include "unit_test.h"
#include "app/app_task_sequencer/app_task_sequencer.h"
#include "mocks/app/app_task_sequencer_init_handler/app_task_sequencer_init_handler.h"
#include "mocks/app/app_task_sequencer_update_handler/app_task_sequencer_update_handler.h"

#define TEST_GROUP ut_app_task_sequencer

/** @utdef{UT-APP-TASK-SEQUENCER-0010 | Test app_task_sequencer_register and app_task_sequencer_init}
    :layout: test
    :tags: su, app_task_sequencer
    :checks: DLT-SRS-APP-TASK-SEQUENCER-ITF-001, DLT-SRS-APP-TASK-SEQUENCER-ITF-002
    :checks_impl: app_task_sequencer_register, app_task_sequencer_init

    - GIVEN a set of initializers registered with app_task_sequencer_register
    - WHEN app_task_sequencer_init is called
    - THEN all initializers are executed in sequence
 @endut*/
TEST(register_and_init_initializer)
{
   app_task_sequencer_init_handler initializers[2] = { app_task_sequencer_init_handler_one,
                                                       app_task_sequencer_init_handler_two };

   app_task_sequencer_register(initializers, 2, 0, 0);

   expect_app_task_sequencer_init_handler_one();
   expect_app_task_sequencer_init_handler_two();

   app_task_sequencer_init();
}

/** @utdef{UT-APP-TASK-SEQUENCER-0020 | Test app_task_sequencer_register and app_task_sequencer_update}
    :layout: test
    :tags: su, app_task_sequencer
    :checks: DLT-SRS-APP-TASK-SEQUENCER-ITF-001, DLT-SRS-APP-TASK-SEQUENCER-ITF-003
    :checks_impl: app_task_sequencer_register, app_task_sequencer_update

    - GIVEN a set of updaters registered with app_task_sequencer_register
    - WHEN app_task_sequencer_update is called
    - THEN all updaters are executed in sequence
 @endut*/
TEST(register_and_update_updaters)
{
   app_task_sequencer_init_handler updaters[2] = { app_task_sequencer_update_handler_one,
                                                   app_task_sequencer_update_handler_two };

   app_task_sequencer_register(0, 0, updaters, 2);

   expect_app_task_sequencer_update_handler_one();
   expect_app_task_sequencer_update_handler_two();

   app_task_sequencer_update();
}

