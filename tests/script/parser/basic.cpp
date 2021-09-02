#include <libevoscript/Parser.h>
#include <iostream>

using namespace evo::script;

int main()
{
    std::cout << "--- Empty ---" << std::endl;
    {
        std::vector<Token> tokens;
        EVOParser parser(tokens);
        auto program = parser.parse_program();
        std::cout << *program << std::endl;
    }

    std::cout << "--- Member + assignment expressions ---" << std::endl;
    {
        std::vector<Token> tokens;
        // (this).testInt = 54;
        tokens.emplace_back(Token::ParenOpen, "(");
        tokens.emplace_back(Token::Name, "this");
        tokens.emplace_back(Token::ParenClose, ")");
        tokens.emplace_back(Token::Dot, ".");
        tokens.emplace_back(Token::Name, "testInt");
        tokens.emplace_back(Token::AssignmentOperator, "=");
        tokens.emplace_back(Token::Number, "54");
        tokens.emplace_back(Token::Semicolon, ";");

        EVOParser parser(tokens);
        auto program = parser.parse_program();
        std::cout << *program << std::endl;
        assert(!program->is_error());
        Runtime runtime;
        std::cout << program->evaluate(runtime) << std::endl;
    }

    std::cout << "--- Function calls + multiple member expressions ---" << std::endl;
    {
        std::vector<Token> tokens;
        // testNull = this.testObject.length();
        tokens.emplace_back(Token::Name, "testNull");
        tokens.emplace_back(Token::AssignmentOperator, "=");
        tokens.emplace_back(Token::Name, "this");
        tokens.emplace_back(Token::Dot, ".");
        tokens.emplace_back(Token::Name, "testObject");
        tokens.emplace_back(Token::Dot, ".");
        tokens.emplace_back(Token::Name, "length");
        tokens.emplace_back(Token::ParenOpen, "(");
        tokens.emplace_back(Token::ParenClose, ")");
        tokens.emplace_back(Token::Semicolon, ";");

        EVOParser parser(tokens);
        auto program = parser.parse_program();
        std::cout << *program << std::endl;
        assert(!program->is_error());
        Runtime runtime;
        std::cout << "program result = " << program->evaluate(runtime) << std::endl;
        std::cout << "testNull = " << runtime.current_execution_context().local_scope_object()->value().get_object()->get("testNull") << std::endl;
    }
}
