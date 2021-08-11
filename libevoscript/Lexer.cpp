#include "Lexer.h"

#include <iostream>
#include <string>

namespace evo::script
{

void Lexer::ignore_whitespace()
{
    consume_while(isspace);
}

char Lexer::consume()
{
    char next = m_input.get();
    if(next == '\n')
    {
        m_location.line++;
        m_location.column = 0;
    }
    else
        m_location.column++;

    return next;
}

bool EVOLexer::lex(std::vector<Token>& output)
{
    bool was_error = false;
    while(!eof())
    {
        char next = peek();
        auto token_start = location();
        if(isalpha(next))
        {
            std::string value = consume_while(isalpha);
            if(value == "this" || value == "null" || value == "undefined")
                output.emplace_back(Token::ReservedKeyword, value, token_start, location());
            else
                output.emplace_back(Token::Name, value, token_start, location());
        }
        else if(isdigit(next))
        {
            std::string value = consume_while(isdigit);
            output.emplace_back(Token::Number, value, token_start, location());
        }
        else if(next == '.')
        {
            consume();
            output.emplace_back(Token::Dot, std::string(&next, 1), token_start, location());
        }
        else if(next == '(')
        {
            consume();
            output.emplace_back(Token::ParenOpen, std::string(&next, 1), token_start, location());
        }
        else if(next == ')')
        {
            consume();
            output.emplace_back(Token::ParenClose, std::string(&next, 1), token_start, location());
        }
        else if(next == '=')
        {
            consume();
            output.emplace_back(Token::AssignmentOperator, std::string(&next, 1), token_start, location());
        }
        else if(next == ';')
        {
            consume();
            output.emplace_back(Token::Semicolon, std::string(&next, 1), token_start, location());
        }
        else
        {
            was_error = true;
            std::cout << "ERROR: Invalid token: " << next << std::endl;
            consume();
            output.emplace_back(Token::Invalid, std::string(&next, 1), token_start, location());
        }
        ignore_whitespace();
    }
    return !was_error;
}

}
