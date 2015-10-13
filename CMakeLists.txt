cmake_minimum_required (VERSION 2.8.5)

########################################################################
#
# Project information
#
########################################################################

project (bind)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/opt/bind" CACHE PATH "Bind install prefix" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Type of build" FORCE)
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS "Configuration type: " ${CMAKE_BUILD_TYPE})

set(BIND_SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")

########################################################################
#
# Options
#
########################################################################

set(BIND_THREADING "auto" CACHE STRINGS "cilk, openmp, none or auto.")
set(BIND_MPI "auto" CACHE STRINGS "disable, single, funneled, serialized, multiple or auto.")
set(BIND_CXX_FLAGS "-std=c++11" CACHE STRINGS "C++11 support is required, e.g. -std=c++11 and -gcc-name=XX")

########################################################################
#
# Configuration
#
########################################################################

set(BIND_INCLUDE_DIRS)
set(BIND_LIBRARIES)
set(BIND_LIBRARY_DIRS)
set(BIND_THREADING_FLAGS)
set(BIND_MPI_FLAGS)

if(${BIND_THREADING} MATCHES "cilk")
  set(BIND_THREADING_FLAGS "-DBIND_THREADING=CILK")
elseif(${BIND_THREADING} MATCHES "openmp")
  find_package(OpenMP REQUIRED)
  set(BIND_THREADING_FLAGS "-DBIND_THREADING=OPENMP ${OpenMP_CXX_FLAGS}")
elseif(${BIND_THREADING} MATCHES "none")
  set(BIND_THREADING_FLAGS "-DBIND_THREADING=NONE")
elseif(NOT ${BIND_THREADING} MATCHES "auto")
  message(FATAL_ERROR "BIND_THREADING has to be one of 'cilk', 'openmp', 'none' or 'auto'")
endif()
message(STATUS "Bind threading: ${BIND_THREADING}")

if(NOT MPI_CXX_FOUND AND NOT ${BIND_MPI} MATCHES "disable")
  find_package(MPI)
endif(NOT MPI_CXX_FOUND AND NOT ${BIND_MPI} MATCHES "disable")
if(MPI_CXX_FOUND AND NOT ${BIND_MPI} MATCHES "disable")
  message(STATUS "MPI include: ${MPI_CXX_INCLUDE_PATH}")
  message(STATUS "MPI libs: ${MPI_CXX_LIBRARIES}")
  list(APPEND BIND_INCLUDE_DIRS ${MPI_CXX_INCLUDE_PATH})
  list(APPEND BIND_LIBRARIES ${MPI_CXX_LIBRARIES})
else(MPI_CXX_FOUND AND NOT ${BIND_MPI} MATCHES "disable")
  message(STATUS "Warning: MPI is disabled")
  set(BIND_MPI "disable")
endif(MPI_CXX_FOUND AND NOT ${BIND_MPI} MATCHES "disable")

if(${BIND_MPI} MATCHES "disable")
  set(BIND_MPI_FLAGS "-DBIND_MPI=MPI_DISABLE")
elseif(${BIND_MPI} MATCHES "single")
  set(BIND_MPI_FLAGS "-DBIND_MPI=MPI_THREAD_SINGLE")
elseif(${BIND_MPI} MATCHES "funneled")
  set(BIND_MPI_FLAGS "-DBIND_MPI=MPI_THREAD_FUNNELED")
elseif(${BIND_MPI} MATCHES "serialized")
  set(BIND_MPI_FLAGS "-DBIND_MPI=MPI_THREAD_SERIALIZED")
elseif(${BIND_MPI} MATCHES "multiple")
  set(BIND_MPI_FLAGS "-DBIND_MPI=MPI_THREAD_MULTIPLE")
elseif(NOT ${BIND_MPI} MATCHES "auto")
  message(FATAL_ERROR "BIND_MPI has to be one of 'disable', 'single', 'funneled', 'serialized', 'multiple' or 'auto'")
endif()
message(STATUS "Bind MPI: ${BIND_MPI}")

include_directories(${BIND_INCLUDE_DIRS} ${BIND_SOURCES_DIR})
link_directories(${BIND_LIBRARY_DIRS})

########################################################################
#
# Tests
#
########################################################################

message("\n\n Bind is a header-only library (so go ahead and 'make install') \n\n")

########################################################################
#
# Configuration dump
#
########################################################################

list(APPEND BIND_CONFIG_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include ${BIND_INCLUDE_DIRS})
list(APPEND BIND_CONFIG_LIBRARY_DIRS ${CMAKE_INSTALL_PREFIX}/lib ${BIND_LIBRARY_DIRS})
list(APPEND BIND_CONFIG_LIBRARIES ${BIND_LIBRARIES})
list(APPEND BIND_CONFIG_COMPILE_FLAGS ${BIND_CXX_FLAGS} ${BIND_THREADING_FLAGS} ${BIND_MPI_FLAGS})

#configure_file(AmbientConfig.cmake.in ${PROJECT_BINARY_DIR}/AmbientConfig.cmake)

########################################################################
#
# Install
#
########################################################################

install(DIRECTORY src/bind src/utils
        DESTINATION include
        COMPONENT headers
        PATTERN .git EXCLUDE
        PATTERN CMakeLists.txt EXCLUDE)

install(DIRECTORY examples
        DESTINATION share
        PATTERN .git EXCLUDE
        PATTERN CMakeLists.txt EXCLUDE)

#install(FILES ${PROJECT_BINARY_DIR}/AmbientConfig.cmake
#        DESTINATION share)

