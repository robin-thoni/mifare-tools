cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

execute_process(COMMAND git log --pretty=format:%h -n 1
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_SHA1)

execute_process(COMMAND git symbolic-ref -q --short HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_REF_NAME)
if ("${GIT_REF_NAME}" EQUAL "")
    execute_process(COMMAND git describe --tags --exact-match
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_REF_NAME)
endif()
string(STRIP "${GIT_REF_NAME}" GIT_REF_NAME)
string(STRIP "${GIT_SHA1}" GIT_SHA1)
add_definitions(-DGIT_SHA1=${GIT_SHA1} -DGIT_REF_NAME=${GIT_REF_NAME})

set(PROJECT_NAME mifare-tools)
project(${PROJECT_NAME})
find_package(GTest)
set(LIBS ${LIBS} nfc freefare)
add_subdirectory(src)
add_subdirectory(cli)
if (GTEST_FOUND)
    enable_testing()
    add_subdirectory(tests)
endif (GTEST_FOUND)