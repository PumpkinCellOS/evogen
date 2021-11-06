#include <evoscript/objects/ASTFunction.h>

#include <evoscript/EscapeSequences.h>
#include <evoscript/Runtime.h>

namespace evo::script
{

Value ASTFunction::call(Runtime& rt, Object const& object, Object& this_, ArgumentList const& args) const
{
    auto scope = rt.scope_object();
    assert(scope);

    auto const& arg_names = object.internal_data<InternalData>().arg_names;
    auto const& body = object.internal_data<InternalData>().body;

    size_t counter = 0;
    for(auto& arg: args)
    {
        if(counter >= arg_names.size())
            // TODO: What to do with extra arguments?
            break;
        scope->allocate(arg_names[counter])->value() = arg;
        counter++;
    }
    return body->evaluate(rt);
}

}
