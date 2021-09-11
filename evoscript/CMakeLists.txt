add_library(libevoscript
    "AbstractOperations.cpp"
    "AST.cpp"
    "CallStack.cpp"
    "ExecutionContext.cpp"
    "Lexer.cpp"
    "Parser.cpp"
    "Runtime.cpp"
    "SourceLocation.cpp"
    "Value.cpp"
    "objects/ASTFunction.cpp"
    "objects/Exception.cpp"
    "objects/GlobalObject.cpp"
    "objects/LocalObject.cpp"
    "objects/Object.cpp"
    "objects/StringObject.cpp"
    "objects/SysObject.cpp"
)

target_include_directories(libevoscript PUBLIC ${CMAKE_SOURCE_DIR})

add_executable(evos
    "main.cpp"
)
target_include_directories(evos PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(evos libevoscript)
