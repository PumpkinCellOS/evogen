#include <evoscript/CallStack.h>
#include <evoscript/Runtime.h>

#include <evoscript/Parser.h>
#include <evoscript/objects/Exception.h>
#include <evoscript/objects/Object.h>
#include <evoscript/objects/SyntaxError.h>

#include <exception>
#include <iostream>

namespace evo::script
{

Runtime::Runtime(std::shared_ptr<GlobalObject> const& global_object, std::shared_ptr<MemoryValue> const& global_this)
: m_global_object(global_object), m_global_this(global_this)
{
    if(!m_global_object)
        m_global_object = std::make_shared<GlobalObject>();
    if(!m_global_this)
        m_global_this = MemoryValue::create_object<Object>();

    m_call_stack.push_execution_context("<global scope>", m_global_this->value().to_object(*this), m_global_object);
}

Runtime::~Runtime()
{
    m_call_stack.pop_execution_context();
}

void Runtime::throw_exception(std::string const& message)
{
    m_exception = std::make_shared<Exception>(*this, message);
}

void Runtime::print_backtrace() const
{
    *m_error_stream << "Backtrace:" << std::endl;
    m_call_stack.print(*m_error_stream);
}

ExecutionContext& Runtime::push_execution_context(std::string const& name, std::shared_ptr<Object> const& this_object)
{
    if(!this_object)
    {
        assert(m_global_this->value().is_object());
        return m_call_stack.push_execution_context(name, m_global_this->value().get_object(), m_global_object);
    }
    return m_call_stack.push_execution_context(name, this_object, m_global_object);
}

ExecutionContext& Runtime::push_scope()
{
    return m_call_stack.push_scope(this_object(), m_global_object);
}

ExecutionContext& Runtime::current_execution_context()
{
    return m_call_stack.current_execution_context();
}

ExecutionContext const& Runtime::current_execution_context() const
{
    return m_call_stack.current_execution_context();
}

void Runtime::pop_execution_context()
{
    m_call_stack.pop_execution_context();
}

Value Runtime::run_code_from_stream(std::istream& input, RunType run_type)
{
    EVOLexer lexer(input);
    std::vector<Token> tokens;
    if(!lexer.lex(tokens))
    {
        if(run_type == RunType::Include)
            throw_exception("Lexer errors in included script");
        else
            std::cerr << "Lexer error :(" << std::endl;
        return {};
    }

    EVOParser parser{tokens};
    auto program = parser.parse_program();
    if(program->is_error())
    {
        if(run_type == RunType::Repl)
        {
            std::cout << "\e[1;31mSyntax Error:\e[m" << std::endl;
            program->errors().print(*m_output_stream, input);
        }
        else
            throw_exception<SyntaxError>(input, program->errors());
        return {};
    }

    *m_error_stream << *program << std::endl;

    auto value = program->evaluate(*this);
    if(run_type != RunType::Include)
    {
        if(has_exception())
        {
            exception()->repl_print(*m_output_stream, true);
            clear_exception();
            return {};
        }
        else
        {
            if(run_type == RunType::Repl)
            {
                // TODO: Do not print escape sequences if not running in tty
                value.value().repl_print(*m_output_stream, true);
                *m_output_stream << std::endl;
            }
        }
    }
    // NOTE: We can omit exception check in include mode since it's 
    // immediately returning script's result without doing anything else.
    return value;
}

Runtime::IdentifierRecord Runtime::resolve_identifier(StringId name) const
{
    // Lookup for already created value in local scope
    auto container = current_execution_context().scope_object();
    auto value = container->get(name);

    // Default to global object if no value is yet created. The
    // access to reference will fail because it's empty.
    if(value.is_invalid())
        return {global_object(), nullptr};
    assert(value.is_reference());
    return {container, value.get_reference()};
}

}
