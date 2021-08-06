add_library(libevogen
    "Block.cpp"
    "Chunk.cpp"
    "Generator.cpp"
    "Task.cpp"
    "Turtle.cpp"
    "World.cpp"
)

target_include_directories(libevogen PUBLIC ${CMAKE_SOURCE_DIR})
