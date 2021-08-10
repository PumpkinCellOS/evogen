#include <libevoscript/AST.h>
#include <libevoscript/Runtime.h>

#include <iostream>

using namespace evo::script;

int main()
{
    Runtime runtime;
    
    // this.testInt
    auto ref1 = std::make_shared<MemberExpression>(std::make_shared<SpecialValue>(SpecialValue::This), "testInt");

    // testReference ( == this )
    auto ref2 = std::make_shared<Identifier>("testReference");

    std::cout << ref1->evaluate(runtime) << std::endl; // == reference to 1234
    std::cout << ref1->evaluate(runtime).to_string(runtime) << std::endl; // 1234
    std::cout << ref2->evaluate(runtime) << std::endl; // == reference to {testInt: 1234}
    std::cout << ref2->evaluate(runtime).to_string(runtime) << std::endl; // [object MapObject]

    // this.testInt = 4321
    auto assignment1 = std::make_shared<AssignmentExpression>(ref1, std::make_shared<IntegerLiteral>(4321));

    // testReference = null ( this = null )
    auto assignment2 = std::make_shared<AssignmentExpression>(ref2, std::make_shared<SpecialValue>(SpecialValue::Null));

    std::cout << assignment1->evaluate(runtime) << std::endl; // reference to 4321
    std::cout << assignment2->evaluate(runtime) << std::endl; // reference null

    // This should fail with exception because of getting member of null object.
    std::cout << ref1->evaluate(runtime) << std::endl; // reference to 4321
    std::cout << "exception: " << runtime.exception_message() << std::endl;
    runtime.clear_exception();

    std::cout << ref2->evaluate(runtime) << std::endl; // reference to null
    std::cout << "exception: " << runtime.exception_message() << std::endl;
    return 0;
}