#include <libevoscript/NativeFunction.h>
#include <libevoscript/Value.h>
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
    if(member == "dump")
        return NativeFunction<ReplObject>::create_value([](Runtime& rt, ReplObject& container, std::vector<Value> const& args)->Value {
            for(auto& value: args)
                std::cout << value.dump_string() << std::endl;

            return Value::new_int(args.size());
        });
    else if(member == "exit")
        return NativeFunction<ReplObject>::create_value([](Runtime& rt, ReplObject& container, std::vector<Value> const& args)->Value {
            auto exit_code = args.size() == 1 ? args[0].to_int(rt) : 0;
            if(rt.has_exception())
                return {};

            container.exit(exit_code);
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
            std::cout << "\e[1m" << value.repl_string() << "\e[0m" << std::endl;
        runtime.clear_exception();

        if(!repl_object->running())
            return repl_object->exit_code();
    }
    return 0;
}
