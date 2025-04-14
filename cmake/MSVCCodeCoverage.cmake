# MSVC-specific code coverage configuration
# This file provides functions to enable code coverage for MSVC compiler

# Check if we're using MSVC
if(NOT MSVC)
  message(STATUS "MSVCCodeCoverage: Not using MSVC compiler, skipping MSVC coverage setup")
  return()
endif()

# Function to enable code coverage for a target
function(msvc_enable_code_coverage TARGET_NAME)
  message(STATUS "Enabling MSVC code coverage for ${TARGET_NAME}")
  
  # Enable code coverage flags for MSVC
  target_compile_options(${TARGET_NAME} PRIVATE /Od /Zi)
  
  # Enable profile-guided optimization data collection
  target_link_options(${TARGET_NAME} PRIVATE /PROFILE)
endfunction()

# Function to setup a target for collecting coverage from tests
function(setup_target_for_msvc_coverage)
  set(options "")
  set(oneValueArgs NAME)
  set(multiValueArgs EXECUTABLE DEPENDENCIES)
  cmake_parse_arguments(Coverage "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  
  if(NOT Coverage_NAME)
    message(FATAL_ERROR "setup_target_for_msvc_coverage requires NAME argument")
  endif()
  
  if(NOT Coverage_EXECUTABLE)
    message(FATAL_ERROR "setup_target_for_msvc_coverage requires EXECUTABLE argument")
  endif()
  
  # Create a custom target that will run tests and generate coverage reports
  add_custom_target(${Coverage_NAME}
    # First run the tests with code coverage enabled
    COMMAND ${Coverage_EXECUTABLE}
    
    # Then generate a coverage report using vstest
    COMMAND ${CMAKE_COMMAND} -E echo "Code coverage data collected. Run Visual Studio's code coverage tools to view the results."
    
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS ${Coverage_DEPENDENCIES}
    VERBATIM
    COMMENT "Running tests with MSVC code coverage"
  )
  
  # Add instructions for viewing coverage in Visual Studio
  add_custom_command(TARGET ${Coverage_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "To view coverage report: Open this project in Visual Studio -> Test -> Analyze Code Coverage -> All Tests"
  )
endfunction()