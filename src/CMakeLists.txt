file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Add the binary tree directory to the search path for linking and include files
link_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories (${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_BINARY_DIR}/src )

add_library (varianthash varianthash.h varianthash.c farmhash64.h farmhash64.c binsearch.h binsearch.c rsidvar.h rsidvar.c)
target_include_directories (varianthash PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Required to link the math library
target_link_libraries(varianthash)
