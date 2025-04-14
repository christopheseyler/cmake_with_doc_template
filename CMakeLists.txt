cmake_minimum_required(VERSION 3.27)
project(Sphin_Cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")



include(GTestHelpers)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOC "Build documentation" ON)



if(BUILD_TESTS)
    enable_testing()
    FetchTestFramework()
endif() 

add_subdirectory(app)




