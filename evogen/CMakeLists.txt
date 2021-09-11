add_library(libevogen
    "Block.cpp"
    "BlockContainer.cpp"
    "BlockStates.cpp"
    "Chunk.cpp"
    "Generator.cpp"
    "Image.cpp"
    "Structure.cpp"
    "Task.cpp"
    "Turtle.cpp"
    "World.cpp"
)

file(DOWNLOAD
    https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
    ${CMAKE_BINARY_DIR}/thirdparty/stb_image.h
    SHOW_PROGRESS
)
target_include_directories(libevogen PRIVATE ${CMAKE_BINARY_DIR}/thirdparty)
target_include_directories(libevogen PUBLIC ${CMAKE_SOURCE_DIR})
