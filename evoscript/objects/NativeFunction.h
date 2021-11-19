#pragma once

#include <evoscript/EscapeSequences.h>
#include <evoscript/Object.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/Exception.h>

namespace evo::script
{

template<class T> requires(std::is_base_of_v<NativeClass<T>, T>)
class NativeFunction : public Class, public NativeClass<NativeFunction<T>>
{
public:
    struct InternalData : public ObjectInternalData
    {
        NativeFunctionCallback<T> callback;

        InternalData(NativeFunctionCallback<T> callback_)
        : callback(callback_) {}
    };

    NativeFunction()
    : Class("NativeFunction") {}

    std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime*, NativeFunctionCallback<T> callback) const
    {
        return std::make_unique<InternalData>(std::move(callback));
    }

    virtual Value call(Runtime& rt, Object& object, Object& this_, ArgumentList const& args) const override
    {
        auto required_class_object = NativeClass<T>::class_object();
        if(!this_.is_instance_of(*required_class_object))
        {
            rt.throw_exception<Exception>("Cannot call function with invalid 'this': '" + this_.type_name() + "', required type: " + required_class_object->name());
            return {};
        }
        return static_cast<NativeObject<T>&>(object).internal_data().callback(rt, this_, args);
    }
};

// NOTE: This must be here because of circular dependency
template<class T>
void Class::define_native_function(StringId name, NativeFunctionCallback<T> callback)
{
    define_virtual_member(name, Object::create_native<NativeFunction<T>>(nullptr, callback));
}

template<class T>
void Object::define_native_function(StringId name, NativeFunctionCallback<T> callback)
{
    auto memval = MemoryValue::create_object(Object::create_native<NativeFunction<T>>(nullptr, callback));
    memval->set_name(name);
    m_members.insert({name, memval});
}

}
