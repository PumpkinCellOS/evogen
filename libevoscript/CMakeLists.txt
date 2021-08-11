add_library(libevoscript
    "AST.cpp"
    "ExecutionContext.cpp"
    "Parser.cpp"
    "Runtime.cpp"
    "Value.cpp"
)

target_include_directories(libevoscript PUBLIC ${CMAKE_SOURCE_DIR})
