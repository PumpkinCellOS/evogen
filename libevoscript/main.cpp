#include <libevoscript/NativeFunction.h>
#include <libevoscript/Value.h>
#include <libevoscript/Lexer.h>
#include <libevoscript/objects/MapObject.h>

#include <fstream>
#include <iostream>
#include <istream>
#include <limits>
#include <sstream>
#include <unistd.h>

using namespace evo::script;

class ReplObject : public GlobalObject
{
public:
    ReplObject();
    virtual Value get(std::string const& member) override;
    virtual std::string type_name() const override { return "ReplObject"; }

    bool running() const { return m_running; }
    int exit_code() const { return m_exit_code; }

private:
    void exit(int code) { m_running = false; m_exit_code = code; }
    std::shared_ptr<MemoryValue> m_dumped;

    bool m_running = true;
    int m_exit_code = 0;
};

ReplObject::ReplObject()
{
    m_dumped = std::make_shared<MemoryValue>();
}

Value ReplObject::get(std::string const& member)
{
    return GlobalObject::get(member);
}

void display_source_range(std::istream& input, SourceSpan const& span)
{
    // TODO: Handle EOF errors
    size_t start = span.start.index - span.start.column;
    input.clear();
    input.seekg(start);

    std::string code;
    if(!std::getline(input, code))
    {
        std::cerr << "(failed to read code)" << std::endl;
        return;
    }
    
    // TODO: Handle multiline
    std::cerr << " | " << code << std::endl << " | ";
    for(size_t s = 0; s < span.start.column; s++)
        std::cerr << " ";

    for(size_t s = 0; s < span.size; s++)
        std::cerr << "^";

    std::cerr << std::endl;
}

bool run_code_from_stream(Runtime& rt, std::istream& input)
{
    EVOLexer lexer(input);
    std::vector<Token> tokens;
    if(!lexer.lex(tokens))
    {
        std::cerr << "Lexer error :(" << std::endl;
        return false;
    }

    for(auto& token: tokens)
        std::cerr << token.value() << "; ";

    std::cerr << std::endl;

    EVOParser parser{tokens};
    auto program = parser.parse_program();
    if(program->is_error())
    {
        std::cerr << "\e[31mSyntax Errors detected:\e[0m" << std::endl;
        for(auto& it: program->errors())
        {
            std::cerr << it.location.start << ": " << it.message << std::endl;
            display_source_range(input, it.location);
        }
        return false;
    }
    std::cerr << *program << std::endl;

    auto value = program->evaluate(rt);
    if(rt.has_exception())
    {
        std::cerr << "\e[31mException: \e[0m" + rt.exception_message() << std::endl;
        rt.clear_exception();
        return false;
    }
    else
    {
        // TODO: Do not print escape sequences if not running in tty
        std::cout << "\e[1m" << value.repl_string() << "\e[0m" << std::endl;
    }
    return true;
}

int main(int argc, char** argv)
{
    auto repl_object = std::make_shared<ReplObject>();
    Runtime runtime{repl_object};

    if(argc == 2)
    {
        auto file = std::ifstream(argv[1]);
        return !run_code_from_stream(runtime, file);
    }
    else if(!isatty(0))
        return run_code_from_stream(runtime, std::cin);

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
        run_code_from_stream(runtime, iss);

        if(!repl_object->running())
            return repl_object->exit_code();
    }
    return 0;
}
