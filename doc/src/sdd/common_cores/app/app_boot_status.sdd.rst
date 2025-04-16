App Boot Status
###############

Purpose
-------

The App Boot Status checks if the core is in a watchdog state or if an unexpected
reboot was encountered. In case of problem it goes into the fatal error state.

Requirements
------------

.. needtable::
    :filter: type == 'srs' and 'dlt' in tags and 'swc' in tags and 'app_boot_status' in tags
    :style: table
    :columns: id;title as "Label";content as "Description"; outgoing as "Uplink(s)" ; is_implemented_by as "Is Implemented By"; is_checked_by as "Is Checked By"
    


Implementation Details
----------------------

.. uml::

   @startuml
   
   package "app_boot_status" {
     [app_boot_status.h] as header
     [app_boot_status.c] as impl
     
     header <-- impl
   }
   
   package "Dependencies" {
     [watchdog.h] as watchdog
     [fatal_error] as fatal_error
     [log.h] as log
   }
   
   impl --> watchdog : uses
   impl --> fatal_error : uses
   impl --> log : uses
   
   note right of impl
     Uses a global variable to track boot status
     0xA55A = indicator of normal boot complete
   end note
   
   @enduml

The app_boot_status component maintains a global variable to track the boot status of the system. This variable is located in specific memory addresses based on the target CPU. When the system starts normally, this variable is set to a magic value (0xA55A). If the system is restarted unexpectedly, this variable will already have this value, indicating an abnormal restart.

The component also checks if the watchdog has been triggered. If so, it raises a fatal error through the fatal error mechanism. If the system has restarted unexpectedly but not due to a watchdog, it logs a critical message.

Design Traceability
-------------------

This section provides the traceability between the requirements and the implementation.

Traceability Summary Table
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. needtable::
    :filter: type == 'impl' and 'dlt' in tags and 'swc' in tags and 'app_boot_status' in tags
    :style: table
    :columns: id; implements as "Implements"; is_checked_by as "Is Checked By"
    :colwidths: 30,40,30


Design Items Description
^^^^^^^^^^^^^^^^^^^^^^^^

.. impl:: app_boot_status_check
   :id: app_boot_status_check
   :layout: impllayout
   :tags: dlt, swc, app_boot_status
   :implements: SU-SRS-APP-00060, SU-SRS-APPFSM-00080
      
   .. code:: cpp

      void app_boot_status_check(void);
   
   The function checks if the core has started normally, going to fatal error state in case of any error.
   
   It performs the following checks:
   - Verifies if watchdog flag is active and raises fatal error if so
   - Checks if the boot status global variable already contains the magic value, which indicates an unexpected restart
   - Sets the boot status global variable to the magic value if this is a normal boot

Tests Suite
-----------

.. needtable::
    :filter: type == 'unittest' and 'app_boot_status' in tags
    :style: table
    :columns: id;title as "Description"; checks as "Validates"
    :colwidths: 10,80,10