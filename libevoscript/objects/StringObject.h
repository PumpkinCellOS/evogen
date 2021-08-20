#pragma once

#include <libevoscript/Value.h>

namespace evo::script
{

class StringObject : public Object
{
public:
    StringObject(std::string str)
    : m_string(str) {}

    virtual Value get(std::string const& member) override;
    virtual std::string type_name() const override { return "String"; }
    virtual std::string dump_string() const override { return "StringObject(" + m_string + ")"; }

private:
    // string string()
    static Value string(Runtime&, StringObject& container, std::vector<Value> const& args);

    // int length()
    static Value length(Runtime&, StringObject& container, std::vector<Value> const& args);

    // string concat(value... args)
    static Value concat(Runtime&, StringObject& container, std::vector<Value> const& args);

    // string substring(int start, int seq_length = -1)
    static Value substring(Runtime&, StringObject& container, std::vector<Value> const& args);

    std::string m_string;
};

}
