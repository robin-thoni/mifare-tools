include_directories(.)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES
        DBO/Result.hxx
        DBO/Result.h
        DataAccess/LibNfc.cpp
        DataAccess/LibNfc.h
        DataAccess/NfcDevice.cpp
        DataAccess/NfcDevice.h
        DataAccess/FreeFareDevice.cpp
        DataAccess/FreeFareDevice.h
        DataAccess/FreeFareTag.cpp
        DataAccess/FreeFareTag.h
        DBO/StringUtils.cpp DBO/StringUtils.h DBO/AccessBitsDbo.cpp DBO/AccessBitsDbo.h Business/LibNfcBusiness.cpp Business/LibNfcBusiness.h Business/NfcDeviceBusiness.cpp Business/NfcDeviceBusiness.h Business/FreeFareDeviceBusiness.cpp Business/FreeFareDeviceBusiness.h Business/FreeFareTagBusiness.cpp Business/FreeFareTagBusiness.h)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
