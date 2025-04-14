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
    :columns: id;title as "Label";content as "Description"; outgoing as "Uplink(s)" 
    :colwidths: 10,12,64,10

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

Design traceability
------------------

.. impl:: Fsm::register_state
   :tags: dnfw, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0010, DNFW-SRS-FSM-0020, DNFW-SRS-FSM-0030, DNFW-SRS-FSM-0040, DNFW-SRS-FSM-0041
   
   .. code:: cpp
   
      template<typename StateIdT, typename... Args>
      void register_state(StateIdT state_id, Args&&... args)
   
   Register a state handler associated to a state identifier.

.. impl:: Fsm::set_initial_state
   :tags: dnfw, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0050
   
   .. code:: cpp
   
      template<typename StateIdT>
      void set_initial_state(StateIdT initial_state_id)
   
   Set the initial state of the FSM.

.. impl:: Fsm::start
   :tags: dnfw, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0060, DNFW-SRS-FSM-0070, DNFW-SRS-FSM-0080
   
   .. code:: cpp
   
      void start()
   
   Start the FSM. This call will trigger the enter behavior of the initial state.
   Throws std::runtime_error if the FSM is already started, if no initial state is set, or if the state is not registered.

.. impl:: Fsm::update
   :tags: dnfw, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0090
   
   .. code:: cpp
   
      bool update()
   
   Update the FSM by calling the update function of the current state.
   Returns True if the FSM is still running, false if the FSM has exited.
   Throws std::runtime_error if the current state has no do function defined for the current state.

.. impl:: Fsm::transition_to
   :tags: dnfw, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0110, DNFW-SRS-FSM-0111
   
   .. code:: cpp
   
      template<typename StateIdT>
      void transition_to(StateIdT next_state_id)
   
   Initiate a transition to a state. This call will trigger the exit behavior of the current state, 
   followed by the enter behavior of the next state.

.. impl:: Fsm::exit
   :tags: dnfw, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0100
   
   .. code:: cpp
   
      void exit()
   
   Request a FSM exit. This call will trigger the exit behavior of the current state before switch on a endless internal *exit* state.

.. impl:: Fsm::is_exit
   :tags: dnfw, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0090
   
   .. code:: cpp
   
      bool is_exit() const
   
   Check if the FSM has exited. Returns True if the FSM has exited, false otherwise.

.. impl:: Fsm::transition_override
   :tags: dnfw, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0120
   
   .. code:: cpp
   
      template <typename StateIdT1, typename StateIdT2, typename StateIdT3>
      void transition_override(StateIdT1 prev_state_id, StateIdT2 next_state_id, StateIdT3 new_next_state_id)
   
   Override a transition to insert a new state in an existing FSM. This call will override the next state id of a transition 
   from a previous state to a next state. It is used to customize the FSM by inserting a new state in the middle of an existing transition.

.. impl:: Fsm::find_state
   :tags: dnfw, swc, fsm
   :layout: impllayout
   :implements: DNFW-SRS-FSM-0130
   
   .. code:: cpp
   
      template <typename StateIdT>
      std::optional<std::shared_ptr<State>> find_state(StateIdT state_id)
   
   Attempts to find a state handler associated to the provided state id. The purpose of this function is to allow the user 
   to access a registered state handler. It could be useful in the case of FSM customization by state overriding, but the 
   overriding state want to execute the original state behavior.

Tests Suite
-----------

.. needtable::
    :filter: type == 'unittest' and 'app' in tags and 'swc' in tags and 'fsm' in tags
    :style: table
    :columns: id;title as "Description"; checks as "Validates"
    :colwidths: 10,80,10