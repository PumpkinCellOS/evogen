#include <evoscript/objects/SyntaxError.h>

namespace evo::script
{

SyntaxError::SyntaxError()
: Class("SyntaxError", NativeClass<Exception>::class_object()) {}

std::unique_ptr<ObjectInternalData> SyntaxError::construct_internal_data(Runtime* rt, std::istream& input, ASTNode::ErrorList const& errors) const
{
    std::ostringstream oss;
    oss << "\e[1;31mSyntax Error\e[m" << std::endl;
    errors.print(oss, input);
    return std::make_unique<InternalData>(oss.str());
}

void SyntaxError::print(Object const& object, std::ostream& output, bool, bool) const
{
    output << object.internal_data<InternalData>().generated_source_display;
}

}
