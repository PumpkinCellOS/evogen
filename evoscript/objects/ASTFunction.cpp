#include <evoscript/objects/ASTFunction.h>

#include <evoscript/Runtime.h>

namespace evo::script
{

Value ASTFunction::call(Runtime& rt, Object&, ArgumentList const& args)
{
    auto scope = rt.scope_object();
    assert(scope);
    size_t counter = 0;
    for(auto& arg: args)
    {
        if(counter >= m_arg_names.size())
            // TODO: What to do with extra arguments?
            break;
        scope->allocate(m_arg_names[counter])->value() = arg;
        counter++;
    }
    return m_body->evaluate(rt);
}

}
