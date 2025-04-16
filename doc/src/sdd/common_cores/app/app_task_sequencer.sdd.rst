App Task Sequencer
##################

Purpose
-------

The `app_task_sequencer` component manages the registration and execution of initialization and update tasks for the application. It ensures that tasks are executed in the correct order during initialization and update phases.

Requirements
------------

.. needtable::
    :filter: type == 'sss' and 'app_task_sequencer' in tags
    :style: table
    :columns: id;title as "Label";content as "Description"; outgoing as "Uplink(s)"
    :colwidths: 10,12,64,10

Implementation Details
----------------------

.. uml::

    @startuml
    [*] --> RegisterTasks : app_task_sequencer_register
    RegisterTasks --> InitTasks : app_task_sequencer_init
    InitTasks --> UpdateTasks : app_task_sequencer_update
    @enduml

Design Traceability
-------------------

Traceability Summary Table
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. needtable::
    :filter: type == 'impl' and 'app_task_sequencer' in tags
    :style: table
    :columns: id; implements as "Implements"; is_checked_by as "Is Checked By"
    :colwidths: 30,40,30

Design Items Description
^^^^^^^^^^^^^^^^^^^^^^^^

.. impl:: app_task_sequencer::app_task_sequencer_register
   :id: app_task_sequencer::app_task_sequencer_register
   :layout: impllayout
   :tags: app_task_sequencer, swc
   :implements: DLT-SRS-APP-TASK-SEQUENCER-ITF-001
   
   .. code:: c

      void app_task_sequencer_register(app_task_sequencer_init_handler* initializers, int num_initializers,
                                       app_task_sequencer_update_handler* updaters, int num_updaters);

   Registers the initialization and update tasks for the application.
   
   Parameters:
     - initializers: Array of function pointers to initialization handlers.
     - num_initializers: Number of initialization handlers in the array.
     - updaters: Array of function pointers to update handlers.
     - num_updaters: Number of update handlers in the array.
   
   Return Value:
     - None (void)

.. impl:: app_task_sequencer::app_task_sequencer_init
   :id: app_task_sequencer::app_task_sequencer_init
   :layout: impllayout
   :tags: app_task_sequencer, swc
   :implements: DLT-SRS-APP-TASK-SEQUENCER-ITF-002
   
   .. code:: c

      void app_task_sequencer_init(void);

   Executes all registered initialization tasks in sequence.
   
   Parameters:
     - None (void)
   
   Return Value:
     - None (void)

.. impl:: app_task_sequencer::app_task_sequencer_update
   :id: app_task_sequencer::app_task_sequencer_update
   :layout: impllayout
   :tags: app_task_sequencer, swc
   :implements: DLT-SRS-APP-TASK-SEQUENCER-ITF-003
   
   .. code:: c
    
      void app_task_sequencer_update(void);

   Executes all registered update tasks in sequence.
   
   Parameters:
     - None (void)
   
   Return Value:
     - None (void)

Tests Suite
-----------

.. needtable::
    :filter: type == 'unittest' and 'app_task_sequencer' in tags
    :style: table
    :columns: id;title as "Description"; checks as "Validates"
    :colwidths: 10,80,10
