add_library (ambient ambient.cpp)

install(TARGETS ambient COMPONENT libraries
                        DESTINATION lib
                        EXPORT AmbientTargets)
