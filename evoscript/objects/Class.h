#pragma once

#include <evoscript/NativeFunction.h>
#include <evoscript/objects/Object.h>
#include <type_traits>

namespace evo::script
{

class Class : public Object
{
public:
    using ConstructorType = std::function<Value(Runtime&, ArgumentList const&)>;

    Class(std::string const& name, ConstructorType&& constructor);

    EVO_OBJECT("Class")

    virtual void repl_print(std::ostream& output, bool print_members) const override;

private:
    Value construct(Runtime&, ArgumentList const& args);

    std::string m_name;
    ConstructorType m_constructor;
};

template<class T>
std::shared_ptr<Class> create_native_class()
{
    static_assert(std::is_base_of_v<Object, T>);
    auto class_ = std::make_shared<Class>(T::static_type_name(), [](Runtime& rt, ArgumentList const& args) {
        return new_object_value_from_args<T>(rt, args);
    });
    T::init_class(*class_);
    return class_;
}

}
