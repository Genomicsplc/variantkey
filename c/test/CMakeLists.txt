# Tests

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Add the binary tree directory to the search path for linking and include files
link_directories(${PROJECT_BINARY_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_BINARY_DIR}/src)

#define macro to simplify adding tests
macro (do_test arg)
  add_test (${arg} ${TARGET_SYSTEM_EMULATOR} ${arg}${CMAKE_EXECUTABLE_SUFFIX})
endmacro ()

# create a smoke test
function(SMOKE_TEST test_name test_file dependencies)
  add_executable(${test_name} ${test_file})
  # Link the executable to the libraries
  target_link_libraries(${test_name} ${dependencies})
  # run test
  do_test (${test_name})
endfunction(SMOKE_TEST)

file (COPY data/test_data.bin DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
SMOKE_TEST (test_binsearch test_binsearch.c variantkey)

file (COPY DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
SMOKE_TEST (test_variantkey test_variantkey.c variantkey)

file (COPY DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
SMOKE_TEST (test_astring test_astring.c variantkey)

file (COPY data/rsvk.10.bin data/vkrs.10.bin DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
SMOKE_TEST (test_rsidvar test_rsidvar.c variantkey)

file (COPY data/vknr.10.bin data/vknr.10.bin DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
SMOKE_TEST (test_nrvk test_nrvk.c variantkey)

file (COPY data/genoref.bin DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
SMOKE_TEST (test_genoref test_genoref.c variantkey)
