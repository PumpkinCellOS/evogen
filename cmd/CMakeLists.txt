add_executable(evogen
    "main.cpp"
)

target_link_libraries(evogen libevogen)
target_include_directories(evogen PRIVATE ${CMAKE_SOURCE_DIR})
