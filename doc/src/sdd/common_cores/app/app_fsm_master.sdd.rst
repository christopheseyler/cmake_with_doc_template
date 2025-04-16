App FSM Master
##############

Purpose
-------

The `app_fsm_master` component manages the finite state machine (FSM) for the application. It transitions between states based on system conditions and ensures proper initialization, synchronization, and updates.

Requirements
------------

.. needtable::
    :filter: type == 'sss' and 'app_fsm_master' in tags
    :style: table
    :columns: id;title as "Label";content as "Description"; outgoing as "Uplink(s)"
    :colwidths: 10,12,64,10

Implementation Details
----------------------

.. uml::

    @startuml
    [*] --> BootCheck : app_fsm_boot_check
    BootCheck --> Initializers : No fatal error
    Initializers --> WaitForStart : Initialization complete
    WaitForStart --> Updaters : Start order received
    WaitForStart --> WaitForStart : Start order not received
    Updaters --> Updaters : No fatal error
    @enduml

Design Traceability
-------------------

Traceability Summary Table
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. needtable::
    :filter: type == 'impl' and 'swc' in tags and 'app_fsm_master' in tags
    :style: table
    :columns: id; implements as "Implements"; is_checked_by as "Is Checked By"
    :colwidths: 30,40,30

Design Items Description
^^^^^^^^^^^^^^^^^^^^^^^^

.. impl:: app_fsm_boot_check
   :id: app_fsm_boot_check
   :layout: impllayout
   :tags: app_fsm_master, swc
   :implements: DLT-SRS-APP-FSM-FUN-001
   
   .. code:: cpp

      void app_fsm_boot_check(void);

   Handles the boot status check and transitions to the `app_fsm_initializers` state if no fatal error occurs.

.. impl:: app_fsm_initializers
   :id: app_fsm_initializers
   :layout: impllayout
   :tags: app_fsm_master, swc
   :implements: DLT-SRS-APP-FSM-FUN-011
   
   .. code:: cpp

      void app_fsm_initializers(void);

   Initializes the application and transitions to the `app_fsm_wait_for_start` state if no fatal error occurs.

.. impl:: app_fsm_wait_for_start
   :id: app_fsm_wait_for_start
   :layout: impllayout
   :tags: app_fsm_master, swc
   :implements: DLT-SRS-APP-FSM-FUN-005
   
   .. code:: cpp

      void app_fsm_wait_for_start(void);

   Waits for the start order and transitions to the `app_fsm_updaters` state if the start order is received.

.. impl:: app_fsm_updaters
   :id: app_fsm_updaters
   :layout: impllayout
   :tags: app_fsm_master, swc
   :implements: DLT-SRS-APP-FSM-FUN-006
   
   .. code:: cpp

      void app_fsm_updaters(void);

   Updates the application and remains in the `app_fsm_updaters` state if no fatal error occurs.

Tests Suite
-----------

.. needtable::
    :filter: type == 'unittest' and 'app_fsm_master' in tags
    :style: table
    :columns: id;title as "Description"; checks as "Validates"
    :colwidths: 10,80,10
