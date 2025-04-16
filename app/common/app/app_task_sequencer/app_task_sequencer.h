#pragma once

/** @file */

#include <stdint.h>

typedef void (*app_task_sequencer_init_handler)(void);
typedef void (*app_task_sequencer_update_handler)(void);

/** The function registers initializer and updater functions list.
 *
 *  @param[in] initializers            Buffer containing initializers
 *  @param[in] initializers_count      Number of initializers in the initializers buffer
 *  @param[in] updaters                Buffer containing updaters
 *  @param[in] updaters_count          Number of initializers in the updaters buffer
 *
 */
void app_task_sequencer_register(const app_task_sequencer_init_handler*   initializers,
                                 uint16_t                                 initializers_count,
                                 const app_task_sequencer_update_handler* updaters,
                                 uint16_t                                 updaters_count);

/** @reqitfdef{DLT-SRS-APP-TASK-SEQUENCER-ITF-002}
 *  App Task Sequencer shall provide a function to call consecutively the initializers.
 *
 *  @requplink{DLT-SRS-APP-FSM-FUN-006}
 */
void app_task_sequencer_init(void);

/** @reqitfdef{DLT-SRS-APP-TASK-SEQUENCER-ITF-003}
 *  App Task Sequencer shall provide a function to call consecutively the updaters.
 *
 *  @requplink{DLT-SRS-APP-FSM-FUN-006}
 */
void app_task_sequencer_update(void);

/** @} */
