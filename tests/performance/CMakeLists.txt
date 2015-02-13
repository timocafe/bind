compile_with_ambient()
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${AMBIENT_SOURCES_DIR}")

add_executable( ambient.gemm ambient.gemm.cpp )
add_executable( ambient.lq   ambient.lq.cpp   )
add_executable( ambient.qr   ambient.qr.cpp   )

foreach( performance_test
         ambient.gemm
         ambient.qr
         ambient.lq
       )
    target_link_libraries (${performance_test} ${AMBIENT_LIBRARIES})
endforeach(performance_test)
