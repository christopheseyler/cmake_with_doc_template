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
    :filter: type == 'req' and 'dnfw' in tags and 'swc' in tags and 'fsm' in tags
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

Tests Suite
-----------

.. needtable::
    :filter: type == 'unittest' and 'app' in tags and 'swc' in tags and 'fsm' in tags
    :style: table
    :columns: id;title as "Description"; checks as "Validates"
    :colwidths: 10,80,10