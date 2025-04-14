app::utils#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utils/Fsm/Fsm.h>

using dartnetfw::core::utils::Fsm;

enum struct StateDefinition
{
    State1,
    State2
};



/** @utdef{UT-FSM-0010 | Starting an empty fsm must throw an exception}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0080
 
    - GIVEN an empty fsm
    - WHEN  the fsm is started
    - THEN  an exception is thrown
 @endut */
TEST(fsm, starting_empty_fsm_throw_exception)
{
    Fsm fsm;
    EXPECT_THROW(fsm.start(), std::runtime_error);
}

/** @utdef{UT-FSM-0020 | Starting an fsm with no initial state must throw an exception}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0080
    
    - GIVEN an fsm with no initial state
    - WHEN the fsm is started
    - THEN an exception is thrown

 @endut */
TEST(fsm, starting_fsm_with_no_initial_state_throw_exception)
{
    Fsm fsm;

    fsm.register_state(StateDefinition::State1, nullptr, nullptr, nullptr);
    EXPECT_THROW(fsm.start(), std::runtime_error);
}


/** @utdef{UT-FSM-0030 | Starting an fsm with an initial state must not throw an exception}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0050, DNFW-SRS-FSM-0060
    
    - GIVEN an fsm with a registered and specified initial state
    - WHEN the fsm is started
    - THEN no exception is thrown

 @endut */
TEST(fsm, starting_fsm_with_initial_state)
{
    Fsm fsm;

    fsm.register_state(StateDefinition::State1, nullptr, nullptr, nullptr);
    fsm.set_initial_state(StateDefinition::State1);
    EXPECT_NO_THROW(fsm.start());
}

class MockState {
public:
    MOCK_METHOD(void, state1_on_enter, (), ());
    MOCK_METHOD(void, state1_on_do, (), ());
    MOCK_METHOD(void, state1_on_exit, (), ());

    MOCK_METHOD(void, state2_on_enter, (), ());
    MOCK_METHOD(void, state2_on_do, (), ());
    MOCK_METHOD(void, state2_on_exit, (), ());
};

/** @utdef{UT-FSM-0040 | Starting an fsm with an initial state must call the enter function of the initial state}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0060, DNFW-SRS-FSM-0070

    - GIVEN an fsm with a registered and specified initial state
    - WHEN the fsm is started
    - THEN only the enter function of the initial state is called

 @endut */
TEST(fsm, starting_fsm_with_initial_state_with_enter_calls_the_enter_function)
{
    Fsm fsm;
   
    MockState mockState;

    fsm.register_state(StateDefinition::State1, 
        [&mockState](){mockState.state1_on_enter();},
        [&mockState](){mockState.state1_on_do();},
        nullptr
    );

    fsm.set_initial_state(StateDefinition::State1);

    EXPECT_CALL(mockState, state1_on_enter()).Times(1);
    EXPECT_CALL(mockState, state1_on_do()).Times(0);
    EXPECT_CALL(mockState, state1_on_exit()).Times(0);

    EXPECT_NO_THROW(fsm.start());
}

/** @utdef{UT-FSM-0050 | Updating a fsm must call the do and function of the current state}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0090

    - GIVEN an fsm with a registered state that defines a do function
    - WHEN the fsm is started and updated
    - THEN The entry function is called once (start()) and the do function is also called once (update())

 @endut */
TEST(fsm, calling_update_calls_do)
{
    Fsm fsm;

    MockState mockState;

    fsm.register_state(StateDefinition::State1, 
        [&mockState](){mockState.state1_on_enter();},
        [&mockState](){mockState.state1_on_do();},
        nullptr
    );

    fsm.set_initial_state(StateDefinition::State1);

    EXPECT_CALL(mockState, state1_on_enter()).Times(1);
    EXPECT_CALL(mockState, state1_on_do()).Times(1);    
    EXPECT_CALL(mockState, state1_on_exit()).Times(0);

    fsm.start();
    EXPECT_NO_THROW(fsm.update());
}
/** @utdef{UT-FSM-0060 | Transitioning to another state must call the exit function of the current state and the enter function of the next state}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0090

    - GIVEN an fsm with a registered state1 and state2, and a current state set to state1
    - WHEN a transition to state2 is requested
    - THEN the exit function of state1 is called, the enter function of state2 is called and the do function of state2 is called on the next update

 @endut */
TEST(fsm, transition_to_state2_calls_state1_exit_and_state2_enter_and_state2_do_on_next_update)
{
    Fsm fsm;

    MockState mockState;

    fsm.register_state(StateDefinition::State1, 
        [&mockState](){mockState.state1_on_enter();},
        [&mockState, &fsm](){mockState.state1_on_do(); fsm.transition_to(StateDefinition::State2);},
        [&mockState](){mockState.state1_on_exit();}
    );

    fsm.register_state(StateDefinition::State2, 
        [&mockState](){mockState.state2_on_enter();},
        [&mockState](){mockState.state2_on_do();},
        [&mockState](){mockState.state2_on_exit();}
    );

    fsm.set_initial_state(StateDefinition::State1);

    EXPECT_CALL(mockState, state1_on_enter()).Times(1);
    EXPECT_CALL(mockState, state1_on_do()).Times(1);    
    EXPECT_CALL(mockState, state1_on_exit()).Times(1);

    EXPECT_CALL(mockState, state2_on_enter()).Times(1);
    EXPECT_CALL(mockState, state2_on_do()).Times(1);    
    EXPECT_CALL(mockState, state2_on_exit()).Times(0);

    fsm.start();
    fsm.update();
    fsm.update();

}

/** @utdef{UT-FSM-0070 | Update must return false to notify the end of the fsm}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0090

    - GIVEN an fsm in exited state
    - WHEN an update() is called
    - THEN the update() function returns false

 @endut */
TEST(fsm, update_must_return_false_when_fsm_is_exited)
{
    Fsm fsm;

    MockState mockState;

    fsm.register_state(StateDefinition::State1, 
        [&mockState](){mockState.state1_on_enter();},
        [&mockState, &fsm](){mockState.state1_on_do(); fsm.exit();},
        [&mockState](){mockState.state1_on_exit();}
    );

    fsm.set_initial_state(StateDefinition::State1);

    EXPECT_CALL(mockState, state1_on_enter()).Times(1);
    EXPECT_CALL(mockState, state1_on_do()).Times(1);
    EXPECT_CALL(mockState, state1_on_exit()).Times(1);
    
    fsm.start();
    
    EXPECT_FALSE(fsm.update());
    
}


/** @utdef{UT-FSM-0080 | Attemps to start an ended fsm must throw an exception}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0090

    - GIVEN an fsm in exited state
    - WHEN the fsm is started
    - THEN an exception is thrown

 @endut */
TEST(fsm, starting_an_exited_fsm_throw_exception)
{
    Fsm fsm;

    MockState mockState;

    fsm.register_state(StateDefinition::State1, 
        [&mockState](){mockState.state1_on_enter();},
        [&mockState, &fsm](){mockState.state1_on_do(); fsm.exit();},
        [&mockState](){mockState.state1_on_exit();}
    );

    fsm.set_initial_state(StateDefinition::State1);

    testing::InSequence seq;

    EXPECT_CALL(mockState, state1_on_enter()).Times(1);
    EXPECT_CALL(mockState, state1_on_do()).Times(1);
    EXPECT_CALL(mockState, state1_on_exit()).Times(1);
    
    fsm.start();
    
    EXPECT_FALSE(fsm.update());

    EXPECT_THROW(fsm.start(), std::runtime_error);
    
}

/** @utdef{UT-FSM-0090 | When a transition is overriden, the fsm must call the enter and do functions of the inserted state}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0090

    - GIVEN an fsm with a registered state1, state2 and a custom state, and a current state set to state1
    - WHEN the transition from state1 to state2 is overriden by the custom state
    - THEN the exit function of state1 is called, the enter function of the custom state is called and the do function of the custom state is called on the next update

 @endut */

TEST(fsm, transition_override)
{
    Fsm fsm;

    MockState mockState;

    fsm.register_state(StateDefinition::State1, 
        
        [&mockState](){mockState.state1_on_enter();},
        [&mockState, &fsm](){mockState.state1_on_do(); fsm.transition_to(StateDefinition::State2);},
        [&mockState](){mockState.state1_on_exit();}
    );

    fsm.register_state(StateDefinition::State2, 
        
        [&mockState](){mockState.state2_on_enter();},
        [&mockState, &fsm](){mockState.state2_on_do();fsm.exit();},
        [&mockState](){mockState.state2_on_exit();}
    );

    fsm.set_initial_state(StateDefinition::State1);


    class MockCustomState 
    {
    public:

        enum struct CustomStateDefinition
        {
            CustomState
        };

        MOCK_METHOD(void, custom_state_on_enter, (), ());
        MOCK_METHOD(void, custom_state_on_do, (), ());
        MOCK_METHOD(void, custom_state_on_exit, (), ());
    };
        

    MockCustomState mockCustomState;

    fsm.register_state(MockCustomState::CustomStateDefinition::CustomState, 
        
        [&mockCustomState](){mockCustomState.custom_state_on_enter();},
        [&mockCustomState, &fsm](){mockCustomState.custom_state_on_do(); fsm.transition_to(StateDefinition::State2);},
        [&mockCustomState](){mockCustomState.custom_state_on_exit();}
    );


    fsm.transition_override(StateDefinition::State1, StateDefinition::State2, MockCustomState::CustomStateDefinition::CustomState);

    testing::InSequence seq;
    
    EXPECT_CALL(mockState, state1_on_enter()).Times(1);
    EXPECT_CALL(mockState, state1_on_do()).Times(1);    
    EXPECT_CALL(mockState, state1_on_exit()).Times(1);

    EXPECT_CALL(mockCustomState, custom_state_on_enter()).Times(1);
    EXPECT_CALL(mockCustomState, custom_state_on_do()).Times(1);
    EXPECT_CALL(mockCustomState, custom_state_on_exit()).Times(1);

    EXPECT_CALL(mockState, state2_on_enter()).Times(1);
    EXPECT_CALL(mockState, state2_on_do()).Times(1);    
    EXPECT_CALL(mockState, state2_on_exit()).Times(1);

    fsm.start();
    while (fsm.update());
    
}

/** @utdef{UT-FSM-0100 | Finding a registered state must return the state}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0090

    - GIVEN an fsm with a registered state
    - WHEN the find_state function is called with the registered state ID
    - THEN the function returns the state

 @endut */
TEST(fsm, find_state_with_registered_state)
{
    Fsm fsm;

    MockState mockState;

    fsm.register_state(StateDefinition::State1, 
        
        [&mockState](){mockState.state1_on_enter();},
        [&mockState, &fsm](){mockState.state1_on_do(); fsm.transition_to(StateDefinition::State2);},
        [&mockState](){mockState.state1_on_exit();}
    );

    auto state = fsm.find_state(StateDefinition::State1);
    EXPECT_TRUE(state.has_value());
}  

/** @utdef{UT-FSM-0110 | Finding an unregistered state must return no state}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0090

    - GIVEN an fsm with a registered state
    - WHEN the find_state function is called with an unregistered state ID
    - THEN the function returns no state

 @endut */
TEST(fsm, find_state_with_unregistered_state)
{
    Fsm fsm;

    MockState mockState;

    fsm.register_state(StateDefinition::State1, 
        
        [&mockState](){mockState.state1_on_enter();},
        [&mockState, &fsm](){mockState.state1_on_do(); fsm.transition_to(StateDefinition::State2);},
        [&mockState](){mockState.state1_on_exit();}
    );

    auto state = fsm.find_state(StateDefinition::State2);
    EXPECT_FALSE(state.has_value());
}

/** @utdef{UT-FSM-0120 | Restarting the fsm with a new initial state set must call the enter function of this new initial state}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0090

    - GIVEN an fsm with a registered state1 and state2, and a current state set to state1
    -   AND the fsm is started and updated until the exit of state2
    - WHEN setting the initial state to state2 and restarting the fsm
    - THEN the fsm restarts and the state2 functions are called

 @endut */
TEST(fsm, restart_after_setting_initial_state_again)
{
    Fsm fsm;

    MockState mockState;

    fsm.register_state(StateDefinition::State1, 
        
        [&mockState](){mockState.state1_on_enter();},
        [&mockState, &fsm](){mockState.state1_on_do(); fsm.transition_to(StateDefinition::State2);},
        [&mockState](){mockState.state1_on_exit();}
    );

    fsm.register_state(StateDefinition::State2, 
        
        [&mockState](){mockState.state2_on_enter();},
        [&mockState, &fsm](){mockState.state2_on_do();fsm.exit();},
        [&mockState](){mockState.state2_on_exit();}
    );

    fsm.set_initial_state(StateDefinition::State1);

    ON_CALL(mockState, state1_on_enter()).WillByDefault(testing::Return());
    ON_CALL(mockState, state1_on_do()).WillByDefault(testing::Return());
    ON_CALL(mockState, state1_on_exit()).WillByDefault(testing::Return());
    ON_CALL(mockState, state2_on_enter()).WillByDefault(testing::Return());
    ON_CALL(mockState, state2_on_do()).WillByDefault(testing::Return());
    ON_CALL(mockState, state2_on_exit()).WillByDefault(testing::Return());

    fsm.start();
    while (fsm.update());
    
    fsm.set_initial_state(StateDefinition::State2);
    
    testing::InSequence seq;
        EXPECT_CALL(mockState, state1_on_enter()).Times(0);
        EXPECT_CALL(mockState, state1_on_do()).Times(0);    
        EXPECT_CALL(mockState, state1_on_exit()).Times(0);

        EXPECT_CALL(mockState, state2_on_enter()).Times(1);
        EXPECT_CALL(mockState, state2_on_do()).Times(1);    
        EXPECT_CALL(mockState, state2_on_exit()).Times(1);

    fsm.start();
    while (fsm.update());
}

/** @utdef{UT-FSM-0130 | Registering a new state with the same state ID must override the previous state}
    :layout: test
    :tags: app, swc, fsm
    :checks: DNFW-SRS-FSM-0130

    - GIVEN an fsm with a registered state associated to a StateDefinition::State1 state ID
    - WHEN registering a new state with the same StateDefinition::State1 state ID
    - THEN the new state functions (entry, do and exit) are called instead of the previous ones

 @endut */

TEST(fsm, state_handler_override)
{
    Fsm fsm;
    MockState mockState;

    fsm.register_state(StateDefinition::State1, 
        
        [&mockState](){mockState.state1_on_enter();},
        [&mockState, &fsm](){mockState.state1_on_do(); fsm.transition_to(StateDefinition::State2);},
        [&mockState](){mockState.state1_on_exit();}
    );

    fsm.set_initial_state(StateDefinition::State1);

    /* The State1 is overriden by another registration*/
    fsm.register_state(StateDefinition::State1, 
        
        [&mockState](){mockState.state2_on_enter();},
        [&mockState, &fsm](){mockState.state2_on_do();fsm.exit();},
        [&mockState](){mockState.state2_on_exit();}
    );
    
    EXPECT_CALL(mockState, state1_on_enter()).Times(0);
    EXPECT_CALL(mockState, state1_on_do()).Times(0);    
    EXPECT_CALL(mockState, state1_on_exit()).Times(0);

    testing::InSequence seq;
        EXPECT_CALL(mockState, state2_on_enter()).Times(1);
        EXPECT_CALL(mockState, state2_on_do()).Times(1);    
        EXPECT_CALL(mockState, state2_on_exit()).Times(1);
  
    fsm.start();
    while (fsm.update());
}