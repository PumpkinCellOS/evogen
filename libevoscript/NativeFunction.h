#pragma once

#include <libevoscript/Runtime.h>
#include <libevoscript/Value.h>
#include <libevoscript/objects/Object.h>

namespace evo::script
{

template<class T>
class NativeFunction : public Function
{
public:
    using ContainerType = T;
    using FunctionType = std::function<Value(Runtime&, ContainerType& container, std::vector<Value> const& args)>;

    NativeFunction(FunctionType&& function)
    : m_function(function) {}

    static Value create_value(FunctionType&& function)
    {
        return Value::new_object(std::make_shared<NativeFunction>(std::move(function)));
    }

    Value call(Runtime& rt, Object& container, std::vector<Value> const& arguments)
    {
        assert(m_function);
        auto this_container = dynamic_cast<ContainerType*>(&container);
        if(!this_container)
        {
            rt.throw_exception("Cannot call function with invalid container '" + container.type_name() + "'");
            return {};
        }
        return m_function(rt, *this_container, arguments);
    }

    virtual std::string type_name() const override { return "NativeFunction"; }

private:
    FunctionType m_function;
};

#define NATIVE_FUNCTION(ContainerType, script_name, internal_name)                                      \
    do {                                                                                                \
        if(member == script_name)                                                                        \
            return Value::new_object(std::make_shared<NativeFunction<ContainerType>>(internal_name));   \
    } while(false)

}
