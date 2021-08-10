#include <libevoscript/Parser.h>
#include <iostream>

using namespace evo::script;

int main()
{
    // Empty
    {
        std::vector<Token> tokens;
        EVOParser parser(tokens);
        auto program = parser.parse_program();
        std::cout << *program << std::endl;
    }

    // Member + assignment expressions
    {
        std::vector<Token> tokens;
        // (this).testInt = 54;
        tokens.emplace_back(Token::ParenOpen, "(");
        tokens.emplace_back(Token::ReservedKeyword, "this");
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
}
