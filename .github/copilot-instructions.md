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

When a test case uses a test vector, the documention of the test shall include a summary table of the vector in RST format like :

    ======================= =============================== ==========================
     Dart Hit Event Type     Details                         Expected Behavior
    ======================= =============================== ==========================
     MissedDartHitEvent      A missed dart                   nb_missed_darts + 1
     DartHitEvent            Double, Outer, Value 1          nb_missed_darts unchanged
    ======================= =============================== ==========================


