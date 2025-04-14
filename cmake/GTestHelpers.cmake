


function (FetchGoogleTest)
    message(STATUS "Adding Google Test Framework")

    set(GTEST_LINKED_AS_SHARED_LIBRARY ON)


  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  include(GoogleTest)
endfunction()

function (FetchTestFramework)
    FetchGoogleTest()
endfunction()


function (gtest_add_unittest unittest_name)
  
  set(options "")
  set(oneValueArgs UT_SOURCE UUT_SOURCE SOURCE_BASE_DIR)
  set(multiValueArgs ADDITIONAL_SOURCES ADDITIONAL_LIBS RESOURCES_DIR UUT_DIR)

  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  if (ARG_UUT_SOURCE)
    list(TRANSFORM ARG_UUT_SOURCE PREPEND ${ARG_SOURCE_BASE_DIR})
  endif()

  if (ARG_ADDITIONAL_SOURCES)
    list(TRANSFORM ARG_ADDITIONAL_SOURCES PREPEND ${ARG_SOURCE_BASE_DIR})
  endif()

  if (NOT DEFINED ARG_UT_SOURCE)
    set(ARG_UT_SOURCE ${unittest_name}.cpp)
  endif()

  add_executable(${unittest_name} ${ARG_UT_SOURCE} ${ARG_UUT_SOURCE} ${ARG_ADDITIONAL_SOURCES})
  add_test(NAME ${unittest_name} COMMAND ${unittest_name})

  target_link_libraries( ${unittest_name}
    ${ARG_ADDITIONAL_LIBS}
    GTest::gtest_main GTest::gtest GTest::gmock
  )
  
  gtest_discover_tests(${unittest_name})
 
endfunction()

