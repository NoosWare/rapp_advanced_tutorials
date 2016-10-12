cmake_minimum_required(VERSION 2.8)
project(say_services)

add_executable(say_services main.cpp)

#RAPP
find_package(rapp REQUIRED)
message(STATUS "libraries: ${RAPP_STATIC_LIBRARIES}")
message(STATUS "headers: ${RAPP_INCLUDE_DIRS}")

#ALDEBARAN
#message(STATUS "$ENV{HOME}")
set(NAOQI ${NAOQI} "$ENV{HOME}/naoqi-sdk-2.1.4.13-linux32/")
set(LIB_PATH ${LIB_PATH} "${NAOQI}/lib")
set(INCLUDE_PATH ${INCLUDE_PATH} "${NAOQI}/include")

find_library(ALPROXIES_LIBRARY NAMES alproxies HINTS ${LIB_PATH})
message(STATUS ${ALPROXIES_LIBRARY})

find_library(ALERROR_LIBRARY NAMES alerror HINTS ${LIB_PATH})
message(STATUS ${ALERROR_LIBRARY})

find_library(ALCOMMON_LIBRARY NAMES alcommon HINTS ${LIB_PATH})
message(STATUS ${ALCOMMON_LIBRARY})

find_library(ALVALUE_LIBRARY NAMES alvalue HINTS ${LIB_PATH})
message(STATUS ${ALVALUE_LIBRARY})

find_library(QI_LIBRARY NAMES qi HINTS ${LIB_PATH})
message(STATUS ${QI_LIBRARY})

find_library(QITYPE_LIBRARY NAMES qitype HINTS ${LIB_PATH})
message(STATUS ${QITYPE_LIBRARY})
include_directories(${INCLUDE_PATH})

#COMMON
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

#If you use a different version of Boost of 1.55,
#then you have to do static link boost(this case)
#If you already have Boost 1.55, then  you only have to add:
#find_package(Boost COMPONENTS system REQUIRED)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
find_package(Boost COMPONENTS system REQUIRED)
if (Boost_FOUND)
	set(Boost_USE_STATIC_LIBS ON)
	set(BUILD_SHARED_LIBS OFF)
	include_directories(${Boost_INCLUDE_DIR})
endif()
message(STATUS ${Boost_LIBRARIES})

set(NAO_LIBRARIES ${ALPROXIES_LIBRARY}
                  ${ALCOMMON_LIBRARY}
                  ${ALERROR_LIBRARY}
                  ${ALVALUE_LIBRARY}
                  ${QI_LIBRARY}
                  ${QITYPE_LIBRARY})

set(RAPP_LIBRARY ${RAPP_STATIC_LIBRARIES} 
                 ${OPENSSL_LIBRARIES} 
				 ${Boost_LIBRARIES}
				 ${CMAKE_THREAD_LIBS_INIT})

#BOTH
target_link_libraries(say_services ${RAPP_LIBRARY}
                                   ${NAO_LIBRARIES})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}-static-libstdc++ -static-libgcc")
set(CMAKE_CXX_FLAGS "-std=gnu++1y -march=atom -mtune=atom -mfpmath=sse")
