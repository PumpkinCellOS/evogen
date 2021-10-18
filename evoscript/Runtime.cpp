#include <evoscript/CallStack.h>
#include <evoscript/Runtime.h>

#include <evoscript/Parser.h>
#include <evoscript/objects/Exception.h>
#include <evoscript/objects/Object.h>

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

    m_call_stack.push_execution_context("<global scope>", m_global_this->value().to_object(*this));
    if(has_exception())
        return;
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
        return m_call_stack.push_execution_context(name, m_global_this->value().get_object());
    }
    return m_call_stack.push_execution_context(name, this_object);
}

ExecutionContext& Runtime::push_scope()
{
    return m_call_stack.push_scope(this_object());
}

ExecutionContext& Runtime::current_execution_context()
{
    return m_call_stack.current_execution_context();
}

void Runtime::pop_execution_context()
{
    m_call_stack.pop_execution_context();
}

void Runtime::display_source_range(std::istream& input, SourceSpan const& span)
{
    // TODO: Handle EOF errors
    size_t start = span.start.index - span.start.column;
    input.clear();
    input.seekg(start);

    std::string code;
    if(!std::getline(input, code))
    {
        *m_output_stream << "(failed to read code)" << std::endl;
        return;
    }
    
    // TODO: Handle multiline
    *m_output_stream << " | " << code << std::endl << " | ";
    for(size_t s = 0; s < span.start.column; s++)
        *m_output_stream << " ";

    for(size_t s = 0; s < span.size; s++)
        *m_output_stream << "^";

    *m_output_stream << std::endl;
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
        if(run_type == RunType::Include)
        {
            // TODO: Add SyntaxError exception
            throw_exception("Parser errors in included script");
        }
        else
        {
            *m_output_stream << "\e[31mSyntax Errors detected:\e[0m" << std::endl;
            for(auto& it: program->errors())
            {
                *m_output_stream << it.location.start << ": " << it.message << std::endl;
                display_source_range(input, it.location);
            }
        }
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

std::shared_ptr<ScopeObject> Runtime::lookup_scope_container_for_member(StringId name) const
{
    return {};
}

}
