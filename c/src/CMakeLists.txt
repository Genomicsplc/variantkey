file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Add the binary tree directory to the search path for linking and include files
link_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories (${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_BINARY_DIR}/src )

add_library (variantkey binsearch.c binsearch.h esid.c esid.h genoref.c genoref.h hex.c hex.h nrvk.c nrvk.h regionkey.c regionkey.h rsidvar.c rsidvar.h variantkey.c variantkey.h)
target_include_directories (variantkey PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Required to link the math library
target_link_libraries(variantkey)
