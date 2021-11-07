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

    m_location.index++;
    return next;
}

bool EVOLexer::lex(std::vector<Token>& output)
{
    ignore_whitespace();
    while(!eof())
    {
        char next = peek();
        auto token_start = location();
        if(isalpha(next) || next == '_')
        {
            consume();
            std::string value = next + consume_while([](char ch) { return isalnum(ch) || ch == '_'; });
            output.emplace_back(Token::Name, value, token_start, location());
        }
        else if(isdigit(next))
        {
            bool is_hex_digit = false;
            std::string value = consume_while([&](char ch) {
                if(ch == 'x')
                {
                    is_hex_digit = true;
                    return true;
                }
                return isdigit(ch) || (is_hex_digit && ((ch >= 'a' && ch <= 'f') || (ch >= 'a' && ch <= 'f')));
            });
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
        else if(next == '{')
        {
            consume();
            output.emplace_back(Token::CurlyOpen, std::string(&next, 1), token_start, location());
        }
        else if(next == '}')
        {
            consume();
            output.emplace_back(Token::CurlyClose, std::string(&next, 1), token_start, location());
        }
        else if(next == '[')
        {
            consume();
            output.emplace_back(Token::BraceOpen, std::string(&next, 1), token_start, location());
        }
        else if(next == ']')
        {
            consume();
            output.emplace_back(Token::BraceClose, std::string(&next, 1), token_start, location());
        }
        else if(next == ':')
        {
            consume();
            output.emplace_back(Token::Colon, std::string(&next, 1), token_start, location());
        }
        else if(next == '=')
        {
            consume();
            char next2 = peek();
            switch(next2)
            {
                case '=':
                    {
                        consume();
                        output.emplace_back(Token::NormalOperator, "==", token_start, location());
                    }
                    break;
                default:
                    output.emplace_back(Token::AssignmentOperator, std::string(&next, 1), token_start, location());
                    break;
            }
        }
        else if(next == '&')
        {
            consume();
            if(peek() == '&')
            {
                consume();
                output.emplace_back(Token::NormalOperator, "&&", token_start, location());
            }
            else
                output.emplace_back(Token::Invalid, std::string(&next, 1), token_start, location());
        }
        else if(next == '|')
        {
            consume();
            if(peek() == '|')
            {
                consume();
                output.emplace_back(Token::NormalOperator, "||", token_start, location());
            }
            else
                output.emplace_back(Token::Invalid, std::string(&next, 1), token_start, location());
        }
        else if(next == '-' || next == '+' || next == '*' || next == '/' || next == '%'
             || next == '!' || next == '~' || next == '<' || next == '>')
        {
            consume();
            char next2 = peek();
            switch(next2)
            {
                case '/':
                    if(next == '/')
                    {
                        consume();
                        consume_while([](char c) { return c != '\n'; });
                    }
                case '+':
                    if(next == '+')
                    {
                        consume();
                        output.emplace_back(Token::NormalOperator, "++", token_start, location());
                    }
                    break;
                case '-':
                    if(next == '-')
                    {
                        consume();
                        output.emplace_back(Token::NormalOperator, "--", token_start, location());
                    }
                    break;
                case '=':
                    if(next == '-' || next == '+' || next == '*' || next == '/' || next == '%')
                    {
                        consume();
                        output.emplace_back(Token::AssignmentOperator, next + std::string("="), token_start, location());
                    }
                    else if(next == '!' || next == '<' || next == '>')
                    {
                        consume();
                        output.emplace_back(Token::NormalOperator, next + std::string("="), token_start, location());
                    }
                    break;
                default:
                    output.emplace_back(Token::NormalOperator, std::string(&next, 1), token_start, location());
                    break;
            }
        }
        else if(next == ';')
        {
            consume();
            output.emplace_back(Token::Semicolon, std::string(&next, 1), token_start, location());
        }
        else if(next == ',')
        {
            consume();
            output.emplace_back(Token::Comma, std::string(&next, 1), token_start, location());
        }
        else if(next == '\"' || next == '\'')
        {
            consume();
            // TODO: Escapes
            std::string literal = consume_while([&](char ch) { return ch != next; });
            if(eof())
            {
                std::cout << "ERROR: Unclosed string literal" << std::endl;
                return false;
            }
            consume(); // Last " or '
            output.emplace_back(Token::String, literal, token_start, location());
        }
        else
        {
            consume();
            output.emplace_back(Token::Invalid, std::string(&next, 1), token_start, location());
        }
        ignore_whitespace();
    }
    return true;
}

}
