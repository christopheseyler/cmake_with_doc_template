# Copilot Instructions


## Requirement syntax using EARS

To write requirements, we use EARS formalism.

This section describes the requirements syntax standard used to define requirements. It complies with the EARS (stands for
*Easy Approach to Requirements Syntax*) standard.
The goal is adopting the defined patterns set to define:

    - Ubiquitous requirements
    - Event-Driven requirements
    - Unwanted Behavior requirements
    - State-Driven requirements
    - Optional Feature requirements
    - Complex requirements

### Ubiquitous Requirements

Ubiquitous requirements are requirements that are always valid:

    - they state a fundamental system property.
    - they don't require any stimulus in order to execute
    - they always exist (not related to any states or events)

The syntax has this form :  `The`  <system name>  `shall` <system response>

### Event-Driven requirements


Event-Driven requirements are requirements that are triggered by an event:

    - They are initialed **when and only when** a trigger occurs or is detected

The syntax has this form : `When` <triggering event> <optional precondition>, `the` <system name> `shall` <system response>

### Unwanted Behavior Requirements


Unwanted Behavior requirements are requirements that describe the system's behavior when an unwanted event occurs:

    - They handle unwanted behaviors including error conditions, failures, faults, disturbances and other undesired events.

The syntax has this form : `If` <unwanted condition or event>, `then the` <system name>`shall` <system response>


### State-Driven Requirements


State-Driven requirements are requirements that are related to a state:

    - they are triggered while the system is in a specific state

The syntax has this form : `While` <system state>,`the` <system name> `shall` <system response>


### Complex Requirements


Complex requirements are requirements that are composed of a combination of the previous types of requirements:

    - they describe complex conditional events involving multiple triggers, states and/or optional features
    - they use a combination of the keywords *When*, *If/Then*, *While* and *Where*

The syntax has this form : <Multiple Conditions>, `the` <system name>`shall` <system response>


Here are some examples:
    
    **When** the landing gear button is depressed once, **if** the software detects that the landing gear does not lock into 
    position, **then the** software **shall** sound an alarm. 

    **Where** a second optical drive is installed, **when** the user selects to copy disks, **the** software **shall** display an option
    to copy directly from one optical drive to the other optical drive.


    **While** in start up mode, **when** the software detects an external flash card, **the** software **shall** use the external flash card 
    to store photos.


## Specification writing

We write high-level specification (SSS) in file *.sss.rst. We write lower-level specification (SRS) in file *.srs.rst

A specification point is represented by an ID, a title (which is kind of a brief description), and the requirement itself (that complies to EARS)

## Tests generation

### Unit test framework

We use google test et google mock for unit testing

### Test cases documentation

The test case documentation uses a specific comment format with this form:

```code

/** @utdef{ <Test_ID> | <a brief description of the test or the scenario>}
    :layout: test
    :tags: dnfw, ut, fsm_handlers, player_turn_ends
    :checks: < the requirement id checked by the test>

    < the test description using the gherkin format like 
    - GIVEN xxx
    - WHEN yyy
    - THEN zzzz
    >
    
 @endut */

```

The Test_ID has the form of UT-MODULE-0010 with the number incremented 10 by 10

### Test cases documentation when the test uses a test vector

When a test case uses a test vector, the documentation of the test shall include a summary table of the vector in RST format like :

    ======================= =============================== ==========================
     Dart Hit Event Type     Details                         Expected Behavior
    ======================= =============================== ==========================
     MissedDartHitEvent      A missed dart                   nb_missed_darts + 1
     DartHitEvent            Double, Outer, Value 1          nb_missed_darts unchanged
    ======================= =============================== ==========================


## Software Components

### Description

A software component is defined either by:
- a class (header file and optionnaly a cpp file) for C++ project
- a couple of C and header file for C project
- a component specification file (component_name.srs.rst) 
- a component detail design file (component_name.sdd.rst)

#### Template

In the following template:
- <xxxx> indicates a placeholder that needs to be replaced
- $$ yyyyy $$ indicates a note to explain something in the document (definition of a section content...) 

#### Structure of the component specification file

The RST file is basically a requirements file that contains the requirements of the component. To write the requirements, the document uses
sphinx-needs extension. It is divided into sections that are defined by the following template:

$$ SRS file template starts here $$

Component Name
##############

Purpose
-------

<Description of the component extracted from the class description in C++ project or from a general comment in the header file for C project>

Requirements
------------

$$ The requirements list uses this format $$

.. list2need:: 
    :types: srs
    :tags: <project>, swc, <software component name>
    :list-options:
        :hide:
    
    * (<requirement id with the form of SU-SRS-<module acronym in upper case>-<number with 5 digits, incremented by 10>>) <Requirement title>. 
    <the requirement text using EARS syntax>
    * ... the next requirement ...


 .. needtable::
    :filter: type == 'sss' and <project> in tags and 'swc' in tags and <software component name> in tags
    :style: table
    :columns: id;title as "Label";content as "Description"; outgoing as "Uplink(s)" 
    :colwidths: 10,12,64,10


$$ SRS file template ends here $$


#### Structure of the component detail design file

The RST file is a design file that provides implementation details of the component. It is divided into sections that are defined by the following template:


$$ SDD file template starts here $$

Component Name
##############

Purpose
-------

<Description of the component extracted from the class description in C++ project or from a general comment in the header file for C project>

Requirements
------------

$$ Remainder of the requirements that drive the project $$

.. needtable::
    :filter: type == 'sss' and <project> in tags and 'swc' in tags and <software component name> in tags
    :style: table
    :columns: id;title as "Label";content as "Description"; outgoing as "Uplink(s)"
    :colwidths: 10,12,64,10

Implementation Details
----------------------

$$ For C++ project, create a plantuml diagram of the class $$
$$ For C project, create a plantuml diagram of the component $$

Design Traceability
-------------------

This section provides the traceability between the requirements and the implementation. 

$$ For each function/method od the module public API, you create an 'implementation entry' following the form of: $$

.. impl:: <module name>::<name of the function / method>
   :id: <module name>::<name of the function / method>
   :layout: impllayout
   :tags: <project name>,  swc, <module name>
   :implements: <link to requirement defined below>
   :is_checked_by: <link to the test case defined below>
   
   .. code:: cpp
		<signature of the function/method>
		
   <Description of the function/method extracted from comment>
   
   <Description of the parameters, return value, exceptions, ... extracted from comment>


$$ To find the link to the requirement, use the unit test associated to this module when the function/method is taken in an ASSERT and/or EXPECT. If found, take the reference to the SRS lin in the unit description (in the :check: field) $$

$$ To find the link to the test case, use the test id associate to the unit test associated to this module when the function/method is taken in an ASSERT and/or EXPECT. $$

Tests Suite
-----------

$$ This section provides the list of tests that are related to the component. It also uses the sphinx-needs extension $$ 

.. needtable::
    :filter: type == 'unittest and <project> in tags and 'swc' in tags and <software component name> in tags
    :style: table
    :columns: id;title as "Description"; checks as "Validates"
    :colwidths: 10,80,10

$$ SDD file template ends here $$



## Documentation template and generation

### System Software Specification documentation (SSS)

This document lists all system requirements allocated to the software. It uses sphinx-needs extension to write down the requirements using this format:

.. list2need::

    :types: sss
    :tags: <project>, <requirement section>
    :list-options:
        :hide:

    * (<requirement id with the form of SU-SSS-<module acronym in upper case>-<number with 5 digits, incremented by 10>>) <Requirement title>. 
    <the requirement text using EARS syntax>
    ((links="<list of links to the URS requirements>"))
    * ... the next requirement ... 

### Software Unit Test and Integration Test documentation (SUITP)

This document uses breathe to extract documentation 