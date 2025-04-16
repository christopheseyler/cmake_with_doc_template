Master vs Slave core
####################


Specialized version of the app is defined depending whether
it is used on the master or the slave core.


Master core (COM) 
=================


The application master module exposes the common application entry point *app_loop()*
 for the master core (COM).

The application master module achieves the next operation:

- Check the boot status
- Initialize the COM app
- Wait for the slave initialization done
- Order the synchronous starting
- Updates the COM app
- Retrieves slave core error

Slave cores (SSM/RAS)
=====================

The application slave module exposes the common application entry point *app_loop()*
 for the slave cores (RAS/SSM).

The application slave module achieves the next operation:

- Check the boot status
- Initialize the RAS/SSM app
- Wait for the synchronous starting
- Update the RAS/SSM app


Requirements
============

Framework for top-level application handling
--------------------------------------------

.. list2need:: 
    :types: srs
    :tags: dlt,  app
    :list-options:
        :hide:
    
    * (SU-SRS-APP-001) Framework for Unified application handling. 
    The app shall provide a unified way to handle the application module.
    * (SU-SRS-APP-001-000) Main loop iteration. 
    The app shall base its main loop iterations according to the forever module.
    * (SU-SRS-APP-00060) Boot status handling. 
    The app fsm shall provide a unified way to handle the boot statuses.
    * (SU-SRS-APP-00070) Fatal error handling. 
    The app fsm shall provide a unified way to handle fatal error.
    * (SU-SRS-APP-00080) Synchronized starting sequence. 
    The app fsm shall provide a unified way to synchronized starting sequence.


.. needtable::
    :filter: type == 'srs' and 'dlt' in tags and 'app' in tags
    :style: table
    :columns: id;title as "Label";content as "Description"; outgoing as "Uplink(s)"
    :colwidths: 10,12,64,10

Components Package 
==================

The app module is also a components package. It is used to manage the app module
 and its sub-components.

.. .. toctree::
   :maxdepth: 1
   
   app_fsm/app_fsm.srs.rst


