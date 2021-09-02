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

bool run_code_from_stream(Runtime& rt, std::istream& input)
{
    EVOLexer lexer(input);
    std::vector<Token> tokens;
    if(!lexer.lex(tokens))
    {
        std::cerr << "Lexer error :(" << std::endl;
        return false;
    }

    EVOParser parser(tokens);
    auto program = parser.parse_program();
    std::cerr << *program << std::endl;
    if(program->is_error())
        return false;

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
    Runtime runtime(MemoryValue::create_existing_object(repl_object));

    if(argc == 2)
    {
        auto file = std::ifstream(argv[1]);
        return run_code_from_stream(runtime, file);
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
