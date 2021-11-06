add_library(evoscript
    "AbstractOperations.cpp"
    "AST.cpp"
    "CallStack.cpp"
    "ExecutionContext.cpp"
    "GlobalObject.cpp"
    "Lexer.cpp"
    "Object.cpp"
    "Parser.cpp"
    "Runtime.cpp"
    "ScopeObject.cpp"
    "SourceLocation.cpp"
    "StringId.cpp"
    "Value.cpp"

    "objects/ASTFunction.cpp"
    "objects/Exception.cpp"
    "objects/String.cpp"
    "objects/SyntaxError.cpp"
    "objects/System.cpp"
    "objects/Time.cpp"
)

target_include_directories(evoscript PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)

add_executable(evos
    "main.cpp"
)
target_include_directories(evos PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(evos evoscript)

install(TARGETS evoscript DESTINATION lib)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/evoscript" DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN "*.inl")
install(TARGETS evoscript EXPORT evoscript DESTINATION lib)
install(FILES ${CMAKE_SOURCE_DIR}/cmake/evoscriptConfig.cmake DESTINATION lib/cmake/evoscript)
install(EXPORT evoscript DESTINATION lib/cmake/evoscript)
