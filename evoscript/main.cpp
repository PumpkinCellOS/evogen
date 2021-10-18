#include <evoscript/NativeFunction.h>
#include <evoscript/Value.h>
#include <evoscript/Lexer.h>
#include <evoscript/objects/Exception.h>
#include <evoscript/objects/Object.h>

#include <fstream>
#include <iostream>
#include <istream>
#include <limits>
#include <sstream>
#include <unistd.h>

using namespace evo::script;

int main(int argc, char** argv)
{
    Runtime runtime;

    if(argc == 2)
    {
        auto file = std::ifstream(argv[1]);
        return runtime.run_code_from_stream(file, Runtime::RunType::Script).is_invalid() ? 1 : 0;
    }
    else if(!isatty(0))
        return runtime.run_code_from_stream(std::cin, Runtime::RunType::Script).is_invalid() ? 1 : 0;

    std::cout << "EvoGen Script REPL" << std::endl;
    while(true)
    {
        std::cout << ">>> \e[32m";
        std::string source;
        if(!std::getline(std::cin, source))
        {
            std::cout << "EOF" << std::endl;
            return 1;
        }
        
        std::cout << "\e[0m";
        std::istringstream iss(source);
        runtime.run_code_from_stream(iss, Runtime::RunType::Repl);
    }
    return 0;
}
