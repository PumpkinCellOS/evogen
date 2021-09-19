#pragma once

#include <evoscript/NativeFunction.h>
#include <evoscript/objects/Object.h>
#include <type_traits>

namespace evo::script
{

class Class : public Object
{
public:
    using ConstructorType = std::function<Value(Runtime&, std::vector<Value> const&)>;

    Class(ConstructorType&& constructor);

    EVO_OBJECT("Class")

private:
    static Value construct(Runtime&, Class&, std::vector<Value> const& args);

   ConstructorType m_constructor;
};

template<class T>
std::shared_ptr<Class> create_native_class()
{
    static_assert(std::is_base_of_v<Object, T>);
    return std::make_shared<Class>([](Runtime& rt, std::vector<Value> const& args) {
        return new_object_value_from_args<T>(rt, args);
    });
}

}