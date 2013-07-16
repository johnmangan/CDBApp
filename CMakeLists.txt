project(DBApplicationLayer)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

find_package(JSONCPP REQUIRED)
find_package(UUID REQUIRED)
find_package(ZMQ)

set (CDBA_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set (CDBA_INC_DIR ${CMAKE_SOURCE_DIR}/include)

set (HEADERS
    ${CDBA_INC_DIR}/cdbapp/ApplicationDatabaseInterface.h
    ${CDBA_INC_DIR}/cdbapp/State.h
    ${CDBA_INC_DIR}/cdbapp/UuidGenerator.h
)

set (SOURCES
    ${CDBA_SRC_DIR}/cdbapp/ApplicationDatabaseInterface.cpp
    ${CDBA_SRC_DIR}/cdbapp/State.cpp
    ${CDBA_SRC_DIR}/cdbapp/UuidGenerator.cpp
    ${CMAKE_SOURCE_DIR}/main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CDBA_INCLUDE_DIR}
    ${UUID_INCLUDE_DIRS}
    ${ZMQ_INCLUDE_DIRS}
)

# Until we get a universal binary version of libuuid, we need to build for i386 in order for inclusion in Max5 external development
# use ccmake instead of setting CMAKE_OSX_ARCHITECTURES below
#set(CMAKE_OSX_ARCHITECTURES i386)

option(BUILD_LIBRARY_AS_STATIC "Builds a static library instead of a dynamiclibrary." OFF)
if(BUILD_LIBRARY_AS_STATIC)
    add_library(cdbapp STATIC ${SOURCES} ${HEADERS})
else(BUILD_LIBRARY_AS_STATIC)
    add_library(cdbapp SHARED ${SOURCES} ${HEADERS})
endif(BUILD_LIBRARY_AS_STATIC)

target_link_libraries(cdbapp
    ${UUID_LIBRARIES}
    ${ZMQ_LIBRARIES}
    ${JSONCPP_LIBRARY}
)

option(EXAMPLE_EXECUTABLE "Will compile an example executable for testing
purposes" OFF)

if(EXAMPLE_EXECUTABLE)
    add_executable(cdbappexe
        ${SOURCES}
        ${HEADERS}
        ${CMAKE_SOURCE_DIR}/main.cpp
    )

    target_link_libraries(cdbappexe
        ${UUID_LIBRARIES}
        ${ZMQ_LIBRARIES}
    )
endif(EXAMPLE_EXECUTABLE)

# I am sure there is a better way to do this
set(INSTALL_LIBDIR "lib")
if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    set(INSTALL_LIBDIR "lib64")
endif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")

# install the library
install (TARGETS cdbapp
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBDIR}
)

#install the headers
install(FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/cdbapp
)

