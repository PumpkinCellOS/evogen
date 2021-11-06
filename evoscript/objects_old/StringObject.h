#pragma once

#include <evoscript/objects/Object.h>

namespace evo::script
{

class StringObject : public Object
{
public:
    StringObject(std::string const& str);
    StringObject(Runtime&, ArgumentList const& args)
    : StringObject(args.get(0).to_string()) {}

    EVO_OBJECT("String")

    virtual std::string dump_string() const override { return "StringObject(" + m_string + ")"; }
    virtual void repl_print(std::ostream& output, bool) const override;
    virtual std::string to_string() const override { return m_string; }
    virtual Value to_primitive(Runtime&, Value::Type) const override;
    virtual Value operator_add(Runtime& rt, Value const& rhs) const override;
    virtual CompareResult operator_compare(Runtime& rt, Value const& rhs) const override;
    virtual Value operator_subscript(Runtime& rt, Value const& rhs) override;

    static void init_class(Class&);

private:
    // int length() const
    Value length(Runtime&, ArgumentList const& args) const;

    // string concat(value... args) const
    Value concat(Runtime&, ArgumentList const& args) const;

    // string substring(int start, int seq_length = -1) const
    Value substring(Runtime&, ArgumentList const& args) const;

    // void append(value... args)
    Value append(Runtime&, ArgumentList const& args);

    std::string m_string;
};

}
