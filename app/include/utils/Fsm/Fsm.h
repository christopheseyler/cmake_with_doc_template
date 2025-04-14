#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <type_traits>
#include <memory>

#include <string_view>
#include <iostream>
#include <functional>
#include <cassert>
#include <optional>

// Detect compiler and define a macro for getting function signature
#if defined(_MSC_VER)  // Microsoft compiler
    #define FUNCTION_SIGNATURE __FUNCSIG__
#elif defined(__GNUC__) || defined(__clang__)  // GCC or Clang
    #define FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#else
    #define FUNCTION_SIGNATURE "unknown_function"
#endif

// Compile-time hashing function (DJB2 hash)
constexpr std::size_t typeHash(std::string_view name) {
    std::size_t hash = 5381;
    for (char c : name)
        hash = ((hash << 5) + hash) + static_cast<std::size_t>(c);
    return hash;
}

namespaceapp::utils 
{

    /** Type erasure class to represents a state by a value built on a hashed value
     *  of the underlying type.
     *  The class is mainly used as a key in the state handlers table.
     **/ 
    class StateId {
    public:

        StateId() : uid(0) {}  // Default value, you may want a different sentinel value
        // Templated constructor to accept any enum class
        template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
        explicit StateId(T state) : uid(getTypeHash<T>() + static_cast<int>(state))  {}

        // Equality operator for unordered_map support
        bool operator==(const StateId& other) const {return uid == other.uid;}

    private:
        int uid;  

        // Generates a unique integer hash for a type at compile-time
        template <typename T>
        static constexpr std::size_t getTypeHash() {return typeHash(FUNCTION_SIGNATURE);}

        // Grant std::hash access to private members
        friend struct std::hash<StateId>;
    };

}

// Specialization of std::hash for StateId
namespace std {
    template <>
    struct hash<app::utils::StateId> {
        std::size_t operator()(const app::utils::StateId& s) const {
            return std::hash<int>()(s.uid);
        }
    };

    template <>
    struct hash<std::pair<app::utils::StateId, app::utils::StateId>> {
        std::size_t operator()(const std::pair<app::utils::StateId, app::utils::StateId>& p) const {
            // Combine hashes of both StateIds
            std::size_t h1 = std::hash<app::utils::StateId>{}(p.first);
            std::size_t h2 = std::hash<app::utils::StateId>{}(p.second);
            
            // Use hash combining technique (from Boost)
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };

}

  
   namespace app::utils 
   {
   
    /**  This component implements a Finite State Machine (FSM) that allows to register a set of states by defining function pointers to their :
         - entry function
         - do function
         - exit function

        After specifying the initial state, the FSM can be started and updated until the FSM goes in an *exit* state. During the update, the FSM calls the *on do* function of the current state.
        The FSM provides a way to initiate a transition to another state. This will call the *on exit* function of the current state and the *on enter* function of the new state.
        Finally, it is possible to manually exit the FSM. This process will call the *on exit* function of the current state before putting the FSM into an endless exit state. 
    */
    class Fsm
    {
    public:
    
        /** Internal object to store a state handler, keeping pointers to function associated to entry, do and exit behavior.
         *  @note The name is used for debugging purposes.
         */
        struct State
        {
            std::string name="unnamed state";
            std::function<void()> on_do=nullptr;
            std::function<void()> on_enter=nullptr;
            std::function<void()> on_exit=nullptr;

            State() = default;
            State(std::function<void()> on_enter, std::function<void()> on_do, std::function<void()> on_exit, const std::string& name = "unnamed state")
                : on_do(on_do), 
                  on_enter(on_enter), 
                  on_exit(on_exit), 
                  name(name)  {}
        };

        Fsm() : current_state_handler(nullptr) 
        {
            set_initial_state(StateId(InternalState::None));
        }
        virtual ~Fsm() = default;
    
        /** Register a state handler associated to a state identifier */
        template<typename StateIdT, typename... Args>
        void register_state(StateIdT state_id, Args&&... args)
        {
            //static_assert(std::is_base_of<GameState, StateT>::value, "State must derive from GameState");
            auto state = std::make_shared<State>(std::forward<Args>(args)...);
            state_handlers_list[StateId(state_id)] = state;
        }
    
        /** Set the initial state of the FSM */
        template<typename StateIdT>
        void set_initial_state(StateIdT initial_state_id)
        {
            current_state_handler = nullptr;
            m_current_state_id = StateId(initial_state_id);
        }

        /** Initiate a transistion to a state.
         * 
         * This call will trigger the exit behavior of the current state, followed by the enter behavior of the next state.
         * 
         * @param next_state_id The state identifier to transition to.
         */
        template<typename StateIdT>
        void transition_to(StateIdT next_state_id)
        {
            auto state_id = StateId(next_state_id);
            transition_to_state_id(state_id);
        }
    
        /** Update the FSM by calling the update function of the current state. 
         * 
         * @return True if the FSM is still running, false if the FSM has exited.
         * @throw std::runtime_error if the current state has no do function defined for the current state.
        */
        bool update()
        {
            if (current_state_handler)
            {
                if (current_state_handler->on_do)
                    current_state_handler->on_do();
                else
                    throw std::runtime_error("No 'do' function defined for state " + current_state_handler->name);
            }
            
            return !is_exit();
        }
    
        /** Start the FSM.
         * 
         * This call will trigger the enter behavior of the initial state.
         * 
         * @throw std::runtime_error if the FSM is already started.
         * @throw std::runtime_error if no initial state is set.
         * 
         */
        void start()
        {


            if (current_state_handler) 
                throw std::runtime_error("Fsm already started");

            if (m_current_state_id == StateId(InternalState::None))
                throw std::runtime_error("No initial state set");

            auto it = state_handlers_list.find(m_current_state_id);
            if (it == state_handlers_list.end())
            {
                throw std::runtime_error("State not registered");
            }
            
            current_state_handler = it->second;

            if (current_state_handler->on_enter)
                current_state_handler->on_enter();
        }


        /** Request a FSM exit.
         * 
         * This call will trigger the exit behavior of the current state before switch on a endless internal *exit* state.
         */
        void exit()
        {
            if (current_state_handler && current_state_handler->on_exit)
                current_state_handler->on_exit();

            current_state_handler = nullptr;
            m_current_state_id = StateId(InternalState::Exit);

        }


        /** Check if the FSM has exited.
         * 
         * @return True if the FSM has exited, false otherwise.
         */
        bool is_exit() const
        {
            return m_current_state_id == StateId(InternalState::Exit);
        }

        /** Override a transistion to insert a new state in an existing FSM.
         * 
         * This call will override the next state id of a transition from a previous state to a next state.
         * It is used to customize the FSM by inserting a new state in the middle of an existing transition.
         * 
         * @param prev_state_id The state identifier of the previous state.
         * @param next_state_id The state identifier of the next state.
         * @param new_next_state_id The state identifier of the new next state.
         * 
         * @note This is the duty of the new state to handle the transition to the original next state.
         */
        template <typename StateIdT1, typename StateIdT2, typename StateIdT3>
        void transition_override(StateIdT1 prev_state_id, StateIdT2 next_state_id, StateIdT3 new_next_state_id)
        {
            transitions_override_list[std::make_pair(StateId(prev_state_id), StateId(next_state_id))] = StateId(new_next_state_id);
        }

        /** Attempts to find a state handler associated to the provided state id.
         * 
         * The purpose of this function is to allow the user to access a registered state handler. 
         * It could be useful in the case of FSM customization by state overriding, but the overriding state want to execute the original state behavior.
         * 
         * @param state_id The state identifier to search for.
         * @return A shared pointer to the state handler if found, otherwise an empty optional.
         */
        template <typename StateIdT>
        std::optional<std::shared_ptr<State>> find_state(StateIdT state_id)
        {
            
            auto it = state_handlers_list.find(StateId(state_id));
            if (it != state_handlers_list.end())
            {
                return it->second;
            }
            return std::nullopt;
        }

    protected:


        void transition_to_state_id(StateId state_id)
        {
            state_id = find_transition_override(m_current_state_id, state_id);
            
            auto it = state_handlers_list.find(state_id);
            if (it == state_handlers_list.end())
            {
                throw std::runtime_error("State not registered");
            } 

            m_current_state_id = state_id;
            
            if (current_state_handler && current_state_handler->on_exit)
                current_state_handler->on_exit();
            
            current_state_handler = it->second;

            

            if (current_state_handler && current_state_handler->on_enter)
                current_state_handler->on_enter();
        }

    private:


        /* Check if a transition override exits. If yes, override the "next state id" by the "overriden one"*/
        StateId find_transition_override(StateId prev_state_id, StateId next_state_id)
        {
            auto it = transitions_override_list.find(std::make_pair(prev_state_id, next_state_id));
            if (it != transitions_override_list.end())
            {
                return it->second;
            }
            return next_state_id;
        }

        std::shared_ptr<State> current_state_handler;
        StateId m_current_state_id{InternalState::None};
        
        std::unordered_map<StateId, std::shared_ptr<State>> state_handlers_list;
        std::unordered_map<std::pair<StateId, StateId>, StateId> transitions_override_list;


        /* Internal state id to identify undefined state or exit state of the FSM*/
        enum struct InternalState
        {
            None, 
            Exit
        };
    };
}