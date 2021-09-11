#include <evoscript/objects/ASTFunction.h>

namespace evo::script
{

Value ASTFunction::call(Runtime& rt, Object&, std::vector<Value> const&)
{
    // TODO: Apply arguments
    return m_body->evaluate(rt);
}

}
