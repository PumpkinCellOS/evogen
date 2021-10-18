#pragma once

#include <evoscript/Runtime.h>
#include <evoscript/Value.h>
#include <evoscript/objects/Object.h>

namespace evo::script
{

template<class T>
class NativeFunction : public Function
{
public:
    using ContainerType = T;
    using FunctionType = std::function<Value(ContainerType* container, Runtime&, std::vector<Value> const& args)>;

    EVO_OBJECT("NativeFunction")

    NativeFunction(StringId name, FunctionType&& function)
    : Function(name), m_function(function) {}

    static Value create_value(FunctionType&& function)
    {
        return Value::new_object(std::make_shared<NativeFunction>(std::move(function)));
    }

    virtual Value call(Runtime& rt, Object& container, std::vector<Value> const& arguments) override
    {
        assert(m_function);
        auto this_container = dynamic_cast<ContainerType*>(&container);
        if(!this_container)
        {
            rt.throw_exception("Cannot call function with invalid container '" + container.type_name() + "'");
            return {};
        }
        return m_function(this_container, rt, arguments);
    }

private:
    FunctionType m_function;
};

#define NATIVE_FUNCTION(ContainerType, script_name, internal_name)                                                      \
    do {                                                                                                                \
        if(member == script_name)                                                                                       \
            return Value::new_object(std::make_shared<NativeFunction<ContainerType>>(script_name, internal_name));      \
    } while(false)

// This must be here due to circular dependencies.
template<class T, class Value>
void Object::define_native_function(StringId script_name, Value const& value)
{
    define_read_only_object_property<NativeFunction<T>>(script_name, script_name, std::forward<Value const>(value));
}

}
