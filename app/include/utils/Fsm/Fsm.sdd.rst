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

.. needtable::
    :filter: type == 'req' and 'app' in tags and 'swc' in tags and 'fsm' in tags
    :style: table
    :columns: id;title as "Label";content as "Description"; outgoing as "Uplink(s)" ; is_implemented_by as "is implemented by"; is_checked_by as "is checked by"
    :colwidths: 10,12,44,10, 10, 10

Implementation Details
----------------------

The FSM implementation follows the classic Finite State Machine pattern with some additional features like state and transition overriding.

.. uml:: 

   @startuml
   
   namespace app::utils {
   
     class StateId {
       - uid: int
       + StateId()
       + StateId(T state)
       + operator==(const StateId&): bool
       - {static} getTypeHash<T>(): size_t
     }
     
    
    package fsm {
      
     class Fsm {
       - current_state_handler: std::shared_ptr<Fsm::State>
       - m_current_state_id: StateId
       - state_handlers_list: std::unordered_map<StateId, std::shared_ptr<Fsm::State>>
       - transitions_override_list: std::unordered_map<std::pair<StateId, StateId>, StateId>
       
       + Fsm()
       + ~Fsm()
       + register_state<StateIdT, Args...>(state_id, args)
       + set_initial_state<StateIdT>(initial_state_id)
       + transition_to<StateIdT>(next_state_id)
       + update(): bool
       + start(): void
       + exit(): void
       + is_exit(): bool
       + transition_override<StateIdT1, StateIdT2, StateIdT3>(prev_state_id, next_state_id, new_next_state_id)
       + find_state<StateIdT>(state_id): std::optional<std::shared_ptr<Fsm::State>>
       # transition_to_state_id(state_id): void
       - find_transition_override(prev_state_id, next_state_id): StateId
     }

     struct Fsm::State {
         + name: std::string
         + on_do: std::function<void()>
         + on_enter: std::function<void()>
         + on_exit: std::function<void()>
         + State()
         + State(on_enter, on_do, on_exit, name)
      }

     enum Fsm::InternalState {
         None,
         Exit
       }
       
   }
    
   }
   @enduml

Key Design Choices:

1. **Type-erased State Identifiers**: The `StateId` class provides type erasure for state IDs, allowing the FSM to work with any enum type as state identifiers.

2. **Function Pointers for State Behaviors**: Each state has three associated behaviors (entry, do, exit), represented by `std::function<void()>` function pointers.

3. **State and Transition Override Mechanism**: The FSM supports customization through:
   - `transition_override`: Allows redirecting transitions to insert new states
   - `find_state`: Allows accessing the original state handler when overriding states

4. **Exception-based Error Handling**: The FSM throws exceptions for various error conditions like:
   - Starting an already started FSM
   - No initial state set
   - State not registered
   - Missing required state functions (e.g., 'do')

5. **Internal State Management**: The FSM maintains an internal state (`None`, `Exit`) to track special states.

FSM Customization
^^^^^^^^^^^^^^^^

The FSM implementation provides two key customization mechanisms: transition override and state handler override.

Transition Override
""""""""""""""""""

The transition override feature allows to modify an existing FSM by inserting new states between existing states. This is particularly useful when the FSM is already defined, and the implementation wants to customize the behavior without modifying the original FSM.

For example, to customize this FSM:

.. plantuml::

    @startuml

    [*]-> state1    
    state1 -> state2
    state2 -> state3
    state3 -> [*]

    @enduml

By inserting a new **new_state** between **state1** and **state2**:

.. plantuml::

    @startuml

    [*]-> state1    
    state1 -> new_state
    new_state -> state2 
    state2 -> state3
    state3 -> [*]

    state new_state #red

    @enduml

It requires the registration of the transition override: <state1, state2> -> new_state.

State Handler Override
"""""""""""""""""""""

The FSM also supports overriding the state handler of a specific state by registering a new state handler with an existing state ID. When a state handler is overridden, the Fsm provides the ability to retrieve the original state handler to access its entry, do, and exit functions. This allows the new state handler to incorporate the original behavior if desired.

Usage Examples
^^^^^^^^^^^^^

Basic FSM Usage
""""""""""""""

The FSM can be used by creating state handlers and registering them:

.. code:: cpp

    class StateHandler
    {
    public:

        StateHandler()  { register_states(); }

        bool update() { return fsm.update(); }
        void start() { fsm.start(); }

    protected:

        enum class StateDefinition
        {
            State1,
            State2
        };

        void state1_on_do() { std::cout << "State1 on do" << std::endl; }
        void state1_on_enter() { std::cout << "State1 on enter" << std::endl; }
        void state1_on_exit() { std::cout << "State1 on exit" << std::endl; }

        void state2_on_do() { std::cout << "State2 on do" << std::endl; }
        void state2_on_enter() { std::cout << "State2 on enter" << std::endl; }
        void state2_on_exit() { std::cout << "State2 on exit" << std::endl; }

        Fsm fsm;

        void register_states()
        {
            fsm.register_state(StateDefinition::State1, 
                [this](){state1_on_enter();},
                [this](){state1_on_do(); fsm.transition_to(StateDefinition::State2);},
                [this](){state1_on_exit();}
            );

            fsm.register_state(StateDefinition::State2, 
                [this](){state2_on_enter();},
                [this](){state2_on_do();},
                [this](){state2_on_exit();}
            );

            fsm.set_initial_state(StateDefinition::State1);
        }
    };

Transition Override Example
""""""""""""""""""""""""""

This example demonstrates how to extend an existing FSM by inserting a new state between two existing states:

.. code:: cpp

    class StateHandlerWithOverride : public StateHandler
    {
    public:

        StateHandlerWithOverride() : StateHandler() { register_my_states(); }

    protected:

        enum struct MyStateDefinition
        {
            NewState
        };

        void register_my_states() 
        {
            fsm.register_state(MyStateDefinition::NewState, 
                [this](){std::cout << "New state on enter" << std::endl; fsm.transition_to(StateDefinition::State2);},
                [this](){std::cout << "New state on do" << std::endl;},
                [this](){std::cout << "New state on exit" << std::endl;}
            );

            fsm.transition_override(StateDefinition::State1, StateDefinition::State2, MyStateDefinition::NewState);
        }
    };

Design Traceability
-------------------

.. impl:: Fsm::register_state
   :id: Fsm::register_state
   :tags: app, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0010, DNFW-SRS-FSM-0020, DNFW-SRS-FSM-0030, DNFW-SRS-FSM-0040, DNFW-SRS-FSM-0041
   
   .. code:: cpp
   
      template<typename StateIdT, typename... Args>
      void register_state(StateIdT state_id, Args&&... args)
   
   Register a state handler associated to a state identifier.
   
   Parameters:
     - state_id: The identifier for the state being registered. Can be any enum type thanks to type erasure.
     - args: Variadic template parameters used to construct the State object, typically containing function pointers for on_enter, on_do, and on_exit behaviors.

.. impl:: Fsm::set_initial_state
   :id: Fsm::set_initial_state
   :tags: app, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0050
      
   .. code:: cpp
   
      template<typename StateIdT>
      void set_initial_state(StateIdT initial_state_id)
   
   Set the initial state of the FSM.
   
   Parameters:
     - initial_state_id: The identifier for the state to be set as the initial state of the FSM.

.. impl:: Fsm::start
   :id: Fsm::start
   :tags: app, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0060, DNFW-SRS-FSM-0070, DNFW-SRS-FSM-0080
   
   .. code:: cpp
   
      void start()
   
   Start the FSM. This call will trigger the enter behavior of the initial state.
   Throws std::runtime_error if the FSM is already started, if no initial state is set, or if the state is not registered.
   
   Parameters: None.

.. impl:: Fsm::update
   :id: Fsm::update
   :tags: app, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0090
   
   .. code:: cpp
   
      bool update()
   
   Update the FSM by calling the update function of the current state.
   Returns True if the FSM is still running, false if the FSM has exited.
   Throws std::runtime_error if the current state has no do function defined for the current state.
   
   Parameters: None.
   
   Returns:
     - bool: True if the FSM is still running, false if the FSM has exited.

.. impl:: Fsm::transition_to
   :id: Fsm::transition_to
   :tags: app, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0110, DNFW-SRS-FSM-0111
   
   .. code:: cpp
   
      template<typename StateIdT>
      void transition_to(StateIdT next_state_id)
   
   Initiate a transition to a state. This call will trigger the exit behavior of the current state, 
   followed by the enter behavior of the next state.
   
   Parameters:
     - next_state_id: The identifier of the state to transition to.

.. impl:: Fsm::exit
   :id: Fsm::exit
   :tags: app, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0100
   
   .. code:: cpp
   
      void exit()
   
   Request a FSM exit. This call will trigger the exit behavior of the current state before switch on a endless internal *exit* state.
   
   Parameters: None.

.. impl:: Fsm::is_exit
   :id: Fsm::is_exit
   :tags: app, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0090
   
   .. code:: cpp
   
      bool is_exit() const
   
   Check if the FSM has exited. Returns True if the FSM has exited, false otherwise.
   
   Parameters: None.
   
   Returns:
     - bool: True if the FSM has exited, false otherwise.

.. impl:: Fsm::transition_override
   :id: Fsm::transition_override
   :tags: app, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0120
   
   .. code:: cpp
   
      template <typename StateIdT1, typename StateIdT2, typename StateIdT3>
      void transition_override(StateIdT1 prev_state_id, StateIdT2 next_state_id, StateIdT3 new_next_state_id)
   
   Override a transition to insert a new state in an existing FSM. This call will override the next state id of a transition 
   from a previous state to a next state. It is used to customize the FSM by inserting a new state in the middle of an existing transition.
   
   Parameters:
     - prev_state_id: The identifier of the source state in the transition to override.
     - next_state_id: The identifier of the destination state in the transition to override.
     - new_next_state_id: The identifier of the new destination state that will replace the original destination.

.. impl:: Fsm::find_state
   :id: Fsm::find_state
   :tags: app, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0130, DNFW-SRS-FSM-0010
   
   .. code:: cpp
   
      template <typename StateIdT>
      std::optional<std::shared_ptr<State>> find_state(StateIdT state_id)
   
   Attempts to find a state handler associated to the provided state id. The purpose of this function is to allow the user 
   to access a registered state handler. It could be useful in the case of FSM customization by state overriding, but the 
   overriding state want to execute the original state behavior.
   
   Parameters:
     - state_id: The identifier of the state to find.
   
   Returns:
     - std::optional<std::shared_ptr<State>>: A shared pointer to the state handler if found, otherwise an empty optional.

Tests Suite
-----------

.. needtable::
    :filter: type == 'unittest' and 'app' in tags and 'swc' in tags and 'fsm' in tags
    :style: table
    :columns: id;title as "Description"; checks as "Validates"
    :colwidths: 10,80,10