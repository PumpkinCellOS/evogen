#pragma once

#include <evoscript/objects/Object.h>

namespace evo::script
{

class StringObject : public Object
{
public:
    StringObject(std::string const& str);
    StringObject(Runtime&, std::vector<Value> const& args)
    : StringObject(args.size() > 0 ? args[0].to_string() : "") {}

    static Value create_value(std::string const& str)
    {
        return Value::new_object(std::make_shared<StringObject>(str));
    }

    EVO_OBJECT("String")

    virtual std::string dump_string() const override { return "StringObject(" + m_string + ")"; }
    virtual void repl_print(std::ostream& output, bool) const override;
    virtual std::string to_string() const override { return m_string; }
    virtual Value to_primitive(Runtime&, Value::Type) const override;
    virtual Value operator_add(Runtime& rt, Value const& rhs) const override;
    virtual CompareResult operator_compare(Runtime& rt, Value const& rhs) const override;
    virtual Value operator_subscript(Runtime& rt, Value const& rhs) override;

private:
    // int length() const
    static Value length(Runtime&, StringObject const& container, std::vector<Value> const& args);

    // string concat(value... args) const
    static Value concat(Runtime&, StringObject const& container, std::vector<Value> const& args);

    // string substring(int start, int seq_length = -1) const
    static Value substring(Runtime&, StringObject const& container, std::vector<Value> const& args);

    // void append(value... args)
    static Value append(Runtime&, StringObject& container, std::vector<Value> const& args);

    std::string m_string;
};

}
