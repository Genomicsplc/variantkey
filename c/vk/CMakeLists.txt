set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/cmd)

# Add the binary tree directory to the search path for linking and include files
link_directories(${PROJECT_BINARY_DIR}/src/variantkey)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_BINARY_DIR}/src/variantkey)

file(COPY DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_executable(vk vk.c)
target_link_libraries(vk variantkey)

# --- PACKAGING ---

install(TARGETS "vk" DESTINATION "bin" COMPONENT "vk")
install(FILES "../../README.md" DESTINATION "share/doc/vk" COMPONENT "vk")
install(FILES "../../VERSION" DESTINATION "share/doc/vk" COMPONENT "vk")
install(FILES "../../RELEASE" DESTINATION "share/doc/vk" COMPONENT "vk")
install(FILES "../../LICENSE" DESTINATION "share/doc/vk" COMPONENT "vk" RENAME copyright)

set(CPACK_PACKAGE_NAME "vk")
set(CPACK_PACKAGE_DESCRIPTION "VariantKey CLI encoder")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VariantKey Fast Encoder Command Line Application")

include(CPack)
