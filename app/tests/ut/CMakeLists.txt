
function (dnfw_add_unittest unittest_name)
  
  set(options "")
  set(oneValueArgs UT_SOURCE UUT_SOURCE SOURCE_BASE_DIR)
  set(multiValueArgs ADDITIONAL_SOURCES ADDITIONAL_LIBS RESOURCES_DIR UUT_DIR)

  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
 
  gtest_add_unittest(
    ${unittest_name}
    UT_SOURCE ${ARG_UT_SOURCE}
    UUT_SOURCE ${ARG_UUT_SOURCE}
    SOURCE_BASE_DIR ${ARG_SOURCE_BASE_DIR}
    ADDITIONAL_SOURCES ${ARG_ADDITIONAL_SOURCES}
    ADDITIONAL_LIBS ${ARG_ADDITIONAL_LIBS} app_headers_only
    RESOURCES_DIR ${ARG_RESOURCES_DIR}
    UUT_DIR ${ARG_UUT_DIR}
  )

 
endfunction()

dnfw_add_unittest(ut_core_utils_fsm)
