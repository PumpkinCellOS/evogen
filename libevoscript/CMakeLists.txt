add_library(libevoscript
    "AbstractOperations.cpp"
    "AST.cpp"
    "ExecutionContext.cpp"
    "Lexer.cpp"
    "Parser.cpp"
    "Runtime.cpp"
    "Value.cpp"
)

target_include_directories(libevoscript PUBLIC ${CMAKE_SOURCE_DIR})
