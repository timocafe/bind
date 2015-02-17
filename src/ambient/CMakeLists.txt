set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${AMBIENT_CXX_FLAGS} ${AMBIENT_THREADING_FLAGS}")

add_library (ambient ambient.cpp)

install(TARGETS ambient COMPONENT libraries
                        DESTINATION lib)
