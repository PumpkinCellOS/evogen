add_library(evoscript
    "AbstractOperations.cpp"
    "AST.cpp"
    "CallStack.cpp"
    "ExecutionContext.cpp"
    "Lexer.cpp"
    "Parser.cpp"
    "Runtime.cpp"
    "SourceLocation.cpp"
    "StringId.cpp"
    "Value.cpp"
    "objects/ASTFunction.cpp"
    "objects/Array.cpp"
    "objects/Class.cpp"
    "objects/Exception.cpp"
    "objects/FsObject.cpp"
    "objects/GlobalObject.cpp"
    "objects/LocalObject.cpp"
    "objects/Object.cpp"
    "objects/StringObject.cpp"
    "objects/SysObject.cpp"
    "objects/TimeObject.cpp"
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
