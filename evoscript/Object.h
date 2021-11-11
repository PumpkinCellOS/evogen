#pragma once

#include <evoscript/AbstractOperations.h>
#include <evoscript/ArgumentList.h>
#include <evoscript/EscapeSequences.h>
#include <evoscript/StringId.h>
#include <evoscript/Value.h>
#include <map>
#include <memory>
#include <sstream>

namespace evo::script
{

struct ObjectInternalData
{
    virtual ~ObjectInternalData() = default;
};

class Object;

using NativeFunctionCallback = std::function<Value(Runtime& rt, Object& t, ArgumentList const& args)>;

template<class T>
class NativeClass
{
public:
    static std::shared_ptr<T> class_object() { static std::shared_ptr<T> class_ = std::make_shared<T>(); return class_; }
};

class Class : public NativeClass<Class>
{
public:
    Class(std::string const& name = "Object", std::shared_ptr<Class> const& base = nullptr)
    : m_name(name), m_base(base) {}

    virtual std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime*, ArgumentList const&) const { return {}; }
    std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime*) const { return {}; }
    virtual void constructor(Runtime&, Object&, ArgumentList const&) const {};
    virtual void destructor(Object&) const {};

    // FIXME: Throw an exception
    virtual Value call(Runtime&, Object const&, Object& this_, ArgumentList const&) const { return Value::undefined(); };

    // function to_string() : string
    virtual std::string to_string(Object const&) const { return "[object " + name() + "]"; }
    // function to_primitive(type: ValueType) : Value
    virtual Value to_primitive(Runtime&, Object const&, Value::Type) const { return {}; }
    // function operator+(rhs: Value) : Value
    virtual Value operator_add(Runtime& rt, Object const&, Value const& rhs) const;
    // function operator<>(rhs: Value) : CompareResult
    virtual CompareResult operator_compare(Runtime& rt, Object const&, Value const& rhs) const;
    // function operator[](rhs: Value) : Value
    virtual Value operator_subscript(Runtime& rt, Object&, Value const& rhs) const;
    // function print(stream: OStream, print_members: bool, dump: bool) : undefined
    virtual void print(Object const&, std::ostream&, bool, bool) const;

    std::string name() const { return m_name; }
    std::shared_ptr<MemoryValue> resolve_class_member(StringId member) const;
    bool is_same_or_base_of(Class&) const;

protected:
    friend class Object;

    void define_virtual_member(StringId name, std::shared_ptr<Object> const& value);

    template<class T>
    void define_native_function(StringId name, NativeFunctionCallback callback);

private:
    std::unordered_map<StringId, std::shared_ptr<Object>> m_vtable;
    std::string m_name;
    std::weak_ptr<Class> m_base;
};

class ClassWrapper : public Class, public NativeClass<ClassWrapper>
{
    struct InternalData : public ObjectInternalData
    {
        std::shared_ptr<Class> underlying_class;

        InternalData(std::shared_ptr<Class> const& uc)
        : underlying_class(uc) {}
    };
public:
    ClassWrapper();

    static std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime*, std::shared_ptr<Class> const& class_)
    {
        return std::make_unique<InternalData>(class_);
    }

    virtual void print(Object const& object, std::ostream& out, bool detailed, bool dump) const override;
    
    template<class T> requires std::is_base_of_v<NativeClass<T>, T>
    static std::shared_ptr<Object> create();
};

class Object
{
public:
    virtual ~Object();

    // TODO: This should be const
    virtual std::shared_ptr<MemoryValue> get(StringId member);
    std::shared_ptr<MemoryValue> get_without_side_effects(StringId member) const;

    static std::shared_ptr<Object> create(Runtime& rt, std::shared_ptr<Class> const& class_, ArgumentList const& args)
    {
        return std::shared_ptr<Object>(new Object(rt, class_, args));
    }

    template<class T, class... Args> requires std::is_base_of_v<NativeClass<T>, T>
    static std::shared_ptr<Object> create_native(Runtime* rt, Args&&... args)
    {
        auto object = std::shared_ptr<Object>(new Object());
        auto class_ = NativeClass<T>::class_object();
        object->m_class = class_;
        object->m_internal_data = static_cast<T&>(*class_).construct_internal_data(rt, std::forward<Args>(args)...);
        // FIXME: Should we call constructor?
        return object;
    }

    std::string type_name() const { return m_class ? m_class->name() : "Object"; }

    template<class T>
    T& internal_data() { assert(m_internal_data); return static_cast<T&>(*m_internal_data.get()); }

    template<class T>
    T const& internal_data() const { assert(m_internal_data); return static_cast<T const&>(*m_internal_data.get()); }

    bool is_instance_of(Class& class_) const;

    std::string dump_string() const { std::ostringstream oss; print_impl(oss, true, true); return oss.str(); }
    void repl_print(std::ostream& output, bool print_members) const;

    // These forwards to underlying class
    std::string to_string() const;
    Value to_primitive(Runtime&, Value::Type) const;
    Value operator_add(Runtime& rt, Value const& rhs) const;
    CompareResult operator_compare(Runtime& rt, Value const& rhs) const;
    Value operator_subscript(Runtime& rt, Value const& rhs);
    Value call(Runtime&, Object& this_, ArgumentList const&) const;

    template<class T>
    void define_native_function(StringId name, NativeFunctionCallback callback);

protected:
    Object() = default;

    std::unordered_map<StringId, std::shared_ptr<MemoryValue>>& members() { return m_members; }
    std::unordered_map<StringId, std::shared_ptr<MemoryValue>> const& members() const { return m_members; }

    // This forwards to underlying class
    void print_impl(std::ostream&, bool print_members, bool dump) const;

    friend class Class;
    friend class ClassWrapper;
    void print_members_impl(std::ostream& output, bool dump) const;

private:
    Object(Runtime& rt, std::shared_ptr<Class> const& class_, ArgumentList const& args);

    std::unordered_map<StringId, std::shared_ptr<MemoryValue>> m_members;
    std::shared_ptr<Class> m_class;
    std::unique_ptr<ObjectInternalData> m_internal_data;
};

template<class T> requires std::is_base_of_v<NativeClass<T>, T>
std::shared_ptr<Object> ClassWrapper::create()
{
    auto class_wrapper = Object::create_native<ClassWrapper>(nullptr, NativeClass<T>::class_object());
    if constexpr(requires(Object& w) { T::init_static_class_members(w); })
        T::init_static_class_members(*class_wrapper);
    return class_wrapper;
}

}
