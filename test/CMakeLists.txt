# Tests

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Add the binary tree directory to the search path for linking and include files
link_directories (${PROJECT_BINARY_DIR}/src)
include_directories (${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_BINARY_DIR}/src)

#define macro to simplify adding tests
macro (do_test arg)
  add_test (${arg} ${TARGET_SYSTEM_EMULATOR} ${arg}${CMAKE_EXECUTABLE_SUFFIX})
endmacro ()

# create a smoke test
function(SMOKE_TEST test_name test_file dependencies)
  add_executable (${test_name} ${CMAKE_CURRENT_BINARY_DIR}/${test_file})
  # Link the executable to the libraries
  target_link_libraries (${test_name} ${dependencies})
  # run test
  do_test (${test_name})
endfunction(SMOKE_TEST)

file (COPY test_farmhash64.c DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
SMOKE_TEST (test_farmhash test_farmhash64.c varianthash)

file (COPY test_varianthash.c DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
SMOKE_TEST (test_varianthash test_varianthash.c varianthash)

