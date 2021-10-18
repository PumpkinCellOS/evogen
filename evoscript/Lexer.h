#pragma once

#include <evoscript/Parser.h>

#include <istream>

namespace evo::script
{

class Lexer
{
public:
    Lexer(std::istream& input)
    : m_input(input) {}

    virtual bool lex(std::vector<Token>& output) = 0;

protected:
    void ignore_whitespace();
    char consume();

    char peek() const { return m_input.peek(); }
    bool eof() const { return m_input.eof() || m_input.fail(); }

    template<class Predicate>
    std::string consume_while(Predicate&& predicate)
    {
        std::string output;
        while(predicate(peek()))
        {
            if(eof())
                return output;
            output += consume();
        }
        return output;
    }

    SourceLocation location() const { return m_location; }

private:
    std::istream& m_input;
    SourceLocation m_location;
};

class EVOLexer : public Lexer
{
public:
    EVOLexer(std::istream& input)
    : Lexer(input) {}

    bool lex(std::vector<Token>& output);
};

}
