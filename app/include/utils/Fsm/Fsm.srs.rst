Fsm
===

Purpose
-------

This component implements a Finite State Machine (FSM) that allows to register a set of states by defining function pointers to their:
- entry function
- do function
- exit function

After specifying the initial state, the FSM can be started and updated until the FSM goes in an *exit* state. During the update, the FSM calls the *on do* function of the current state.
The FSM provides a way to initiate a transition to another state. This will call the *on exit* function of the current state and the *on enter* function of the new state.
Finally, it is possible to manually exit the FSM. This process will call the *on exit* function of the current state before putting the FSM into an endless exit state.

Requirements
------------

.. list2need::
    :types: req
    :tags: app, srs, swc, fsm
    :list-options:
        :hide:    

    * (DNFW-SRS-FSM-0010) State Registration. The fsm shall allow to register a state based on an value and a set of function pointers. ((links="DNFW-SSS-GAMEFSM-00020"))
    * (DNFW-SRS-FSM-0020) Support of *on enter* function. While state registration, the fsm shall allow to define a function pointer that will be called when the state is entered.((links="DNFW-SSS-GAMEFSM-00021"))
    * (DNFW-SRS-FSM-0030) Support of *on do* function. While state registration, the fsm shall allow to define a function pointer that will be called when the state is active.((links="DNFW-SSS-GAMEFSM-00021"))
    * (DNFW-SRS-FSM-0040) Support of *on exit* function. While state registration, the fsm shall allow to define a function pointer that will be called when the state is exited.((links="DNFW-SSS-GAMEFSM-00021"))
    * (DNFW-SRS-FSM-0041) Support of incomplete state function registration. While state registration, the fsm shall support that some state functions are not defined (nullptr). ((no_uplink="Implementation choice to support partial state handler definition"))
    * (DNFW-SRS-FSM-0050) Initial State setup. The fsm shall allow to define the initial state of the machine. ((links="DNFW-SSS-GAMEFSM-00030"))
    * (DNFW-SRS-FSM-0060) Starting FSM. The fsm shall allow to start the machine. ((links="DNFW-SSS-GAMEFSM-00030"))
    * (DNFW-SRS-FSM-0070) Starting a FSM calls the *on enter* function of the initial state. When starting the FSM, the fsm shall call the on_enter() function of the initial state. ((links="DNFW-SSS-GAMEFSM-00031"))
    * (DNFW-SRS-FSM-0080) Error handling on FSM Starting. When Starting the FSM, If an error is detected, the fsm shall throw an exception. ((no_uplink="Implementation choice for error handling management"))
    * (DNFW-SRS-FSM-0090) FSM update calls the *on do* function of the current state. The fsm shall allow to update the machine by calling the *on do* function of the current state. ((links="DNFW-SSS-GAMEFSM-00035, DNFW-SSS-GAMEFSM-00036"))
    * (DNFW-SRS-FSM-0100) FSM exit calls the *on exit* function of the current state. The fsm shall allow to exit the machine by calling the *on exit* function of the current state. ((links="DNFW-SSS-GAMEFSM-00050"))
    * (DNFW-SRS-FSM-0110) FSM transition. The fsm shall allow to initiate a transition from the current state to another state. ((links="DNFW-SSS-GAMEFSM-00040")) 
    * (DNFW-SRS-FSM-0111) FSM transition action. When a transition is requested, the fsm shall call the *on exit* function of the current state and the *on enter* function of the new state. ((links="DNFW-SSS-GAMEFSM-00041")) 
    * (DNFW-SRS-FSM-0120) FSM transition override. The fsm shall allow to override a transition from a state to another state. ((links="DNFW-SSS-GAMEFSM-00100"))
    * (DNFW-SRS-FSM-0130) FSM state override. The fsm shall allow to override a state by another. ((links="DNFW-SSS-GAMEFSM-00110")) 
    
.. needtable::
    :filter: 'app' in tags and 'srs' in tags and 'swc' in tags and 'fsm' in tags
    :style: table
    :columns: id;title as "Label";content as "Description"; outgoing as "Uplink(s)"; incoming as "Implemented by"
    :colwidths: 10,8,62,10,10