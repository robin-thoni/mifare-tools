include_directories(.)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES
        DBO/Result.hxx
        DBO/Result.h
        Business/LibNfc.cpp
        Business/LibNfc.h
        Business/NfcDevice.cpp
        Business/NfcDevice.h
        Business/FreeFareDevice.cpp
        Business/FreeFareDevice.h
        Business/FreeFareTag.cpp
        Business/FreeFareTag.h
        Business/StringUtils.cpp Business/StringUtils.h DBO/AccessBitsDbo.cpp DBO/AccessBitsDbo.h)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
