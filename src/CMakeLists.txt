add_executable(random_sim random_sim.cpp)
set_target_properties(random_sim PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

target_link_libraries(random_sim PRIVATE blockchain)