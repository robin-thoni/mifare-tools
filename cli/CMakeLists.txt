include_directories(.)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb -g3")
include_directories(. ../src)
set(SOURCE_FILES
        main.cpp
        Interface/MainClass.cpp
        Interface/MainClass.h
        Interface/CommandLineParser.cpp
        Interface/CommandLineParser.h
        DBO/CommandLineOption.cpp
        DBO/CommandLineOption.h
        )

set(LIBS ${LIBS} ${PROJECT_NAME})
add_executable(${PROJECT_NAME}-cli ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}-cli ${LIBS})
