#include "libevoscript/Value.h"
#include <libevoscript/Lexer.h>

#include <iostream>
#include <limits>
#include <sstream>

using namespace evo::script;

class ReplObject : public MapObject
{
public:
    ReplObject();
    virtual Value get(std::string const& member) override;
    virtual std::string type_name() const override { return "ReplObject"; }

    bool running() const { return m_running; }

private:
    void exit() { m_running = false; }
    std::shared_ptr<MemoryValue> m_dumped;

    bool m_running = true;
};

ReplObject::ReplObject()
{
    m_dumped = std::make_shared<MemoryValue>();
}

Value ReplObject::get(std::string const& member)
{
    if(member == "dumped")
        return Value::new_reference(m_dumped);
    else if(member == "dump")
        return NativeFunction::create_value([](Runtime& rt)->Value {
            // TODO: dump() should take an argument if it will be possible.
            std::cout << rt.this_object<ReplObject>()->m_dumped->dump_string() << std::endl;
            return Value::new_int(1);
        });
    else if(member == "exit")
        return NativeFunction::create_value([](Runtime& rt)->Value {
            rt.this_object<ReplObject>()->exit();
            return Value::undefined();
        });
    else
        return MapObject::get(member);
}

int main()
{
    auto repl_object = std::make_shared<ReplObject>();
    Runtime runtime(MemoryValue::create_existing_object(repl_object));

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
        EVOLexer lexer(iss);
        std::vector<Token> tokens;
        if(!lexer.lex(tokens))
        {
            std::cout << "Lexer error :(" << std::endl;
            continue;
        }

        EVOParser parser(tokens);
        auto program = parser.parse_program();
        std::cout << *program << std::endl;
        if(program->is_error())
            continue;
    
        auto value = program->evaluate(runtime);
        if(runtime.has_exception())
            std::cout << "\e[31mException: \e[0m" + runtime.exception_message() << std::endl;
        else
            std::cout << "\e[1m" << value.to_string(runtime) << "\e[0m" << std::endl;
        runtime.clear_exception();

        if(!repl_object->running())
            return 0;
    }
    return 0;
}
