#include "app/app_task_sequencer/app_task_sequencer.h"

#include <stdint.h>

static const app_task_sequencer_init_handler*   app_task_sequencer_initializers;
static uint16_t                                 app_task_sequencer_initializers_count;
static const app_task_sequencer_update_handler* app_task_sequencer_updaters;
static uint16_t                                 app_task_sequencer_updaters_count;

void app_task_sequencer_register(const app_task_sequencer_init_handler*   initializers,
                                 uint16_t                                 initializers_count,
                                 const app_task_sequencer_update_handler* updaters,
                                 uint16_t                                 updaters_count)
{
   app_task_sequencer_initializers = initializers;
   app_task_sequencer_initializers_count = initializers_count;
   app_task_sequencer_updaters = updaters;
   app_task_sequencer_updaters_count = updaters_count;
}


void app_task_sequencer_init(void)
{
   /* Iterate in the initializer list to call the registered callback */
   for (uint16_t initializer_index = 0; initializer_index < app_task_sequencer_initializers_count;
        initializer_index++)
   {
      app_task_sequencer_initializers[initializer_index]();
   }
}

void app_task_sequencer_update(void)
{
   /* Iterate in the updaters list to call the registered callback */
   for (uint32_t updater_index = 0; updater_index < app_task_sequencer_updaters_count;
        updater_index++)
   {
      app_task_sequencer_updaters[updater_index]();
   }
}
