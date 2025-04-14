Requirements Syntax Standard - EARS
###################################

Purpose
=======

This section describes the requirements syntax standard used to define requirements. It complies with the EARS (stands for
*Easy Approach to Requirements Syntax*) standard.
The goal is adopting the defined patterns set to define:

    - Ubiquitous requirements
    - Event-Driven requirements
    - Unwanted Behavior requirements
    - State-Driven requirements
    - Optional Feature requirements
    - Complex requirements

Ubiquitous Requirements
=======================

Ubiquitous requirements are requirements that are always valid:

    - they state a fundamental system property.
    - they don't require any stimulus in order to execute
    - they always exist (not related to any states or events)

.. admonition:: Syntax

    :guilabel:`The` *<system name>* :guilabel:`shall` *<system response>*

Event-Driven requirements
=========================

Event-Driven requirements are requirements that are triggered by an event:

    - They are initialed **when and only when** a trigger occurs or is detected

.. admonition:: Syntax
    
    :guilabel:`When` *<triggering event>* *<optional precondition>*, :guilabel:`the` *<system name>* :guilabel:`shall` *<system response>*

Unwanted Behavior Requirements
==============================

Unwanted Behavior requirements are requirements that describe the system's behavior when an unwanted event occurs:

    - They handle unwanted behaviors including error conditions, failures, faults, disturbances and other undesired events.

.. admonition:: Syntax

    :guilabel:`If` *<unwanted condition or event>*, :guilabel:`then the` *<system name>* :guilabel:`shall` *<system response>*


State-Driven Requirements
=========================

State-Driven requirements are requirements that are related to a state:

    - they are triggered while the system is in a specific state

.. admonition:: Syntax

    :guilabel:`While` *<system state>*, :guilabel:`the` *<system name>* :guilabel:`shall` *<system response>*

Optional Feature Requirements
=============================

Optional Feature requirements are requirements that depends on optional features:

    - they are invoked **only** in systems that include the particular **optional** feature

.. admonition:: Syntax

    :guilabel:`Where` *<feature is included>*, :guilabel:`the` *<system name>* :guilabel:`shall` *<system response>*


Complex Requirements
====================

Complex requirements are requirements that are composed of a combination of the previous types of requirements:

    - they describe complex conditional events involving multiple triggers, states and/or optional features
    - they use a combination of the keywords *When*, *If/Then*, *While* and *Where*

.. admonition:: Syntax

    *<Multiple Conditions>*, :guilabel:`the` *<system name>* :guilabel:`shall` *<system response>*


Here are some examples:
    
    **When** the landing gear button is depressed once, **if** the software detects that the landing gear does not lock into 
    position, **then the** software **shall** sound an alarm. 

    **Where** a second optical drive is installed, **when** the user selects to copy disks, **the** software **shall** display an option
    to copy directly from one optical drive to the other optical drive.


    **While** in start up mode, **when** the software detects an external flash card, **the** software **shall** use the external flash card 
    to store photos.
