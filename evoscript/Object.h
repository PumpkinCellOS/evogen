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
class NativeObject;

// TODO: Remove this enable_shared_from_this
template<class T>
class NativeClass : public std::enable_shared_from_this<NativeClass<T>>
{
public:
    static std::shared_ptr<T> class_object() { static std::shared_ptr<T> class_ = std::make_shared<T>(); return class_; }

    std::shared_ptr<NativeObject<T>> create_object(Runtime& rt, ArgumentList const& constructor_args) const;

    template<class... Args>
    std::shared_ptr<NativeObject<T>> create_object(Runtime& rt, Args&&... args) const;

private:
    virtual void constructor(Runtime&, NativeObject<T>&, ArgumentList const&) const {};
};

class Class : public NativeClass<Class>
{
public:
    Class(std::string const& name = "Object", std::shared_ptr<Class> const& base = nullptr)
    : m_name(name), m_base(base) {}

    virtual void destructor(Object&) const {};

    // FIXME: Throw an exception
    virtual Value call(Runtime&, Object const&, Object& this_, ArgumentList const&) const { return Value::undefined(); };

    // function to_string() : string
    static StringId to_string_sid;
    // function to_primitive(type: ValueType) : Value
    static StringId to_primitive_sid;
    // function operator+(rhs: Value) : Value
    static StringId op_add_sid;
    // function operator<>(rhs: Value) : CompareResult
    static StringId op_compare_sid;
    // function operator[](rhs: Value) : Value
    static StringId op_subscript_sid;
    // function print(stream: OStream, print_members: bool, dump: bool) : undefined
    static StringId print_sid;

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

class Object : public std::enable_shared_from_this<Object>
{
public:
    virtual ~Object();

    Object(Object const& other) = delete;

    // TODO: This should be const
    virtual std::shared_ptr<MemoryValue> get(StringId member);
    std::shared_ptr<MemoryValue> get_without_side_effects(StringId member) const;

    static std::shared_ptr<Object> create(Runtime& rt, std::shared_ptr<Class> const& class_, ArgumentList const& args);

    template<class T, class... Args> requires std::is_base_of_v<NativeClass<T>, T>
    static std::shared_ptr<NativeObject<T>> create_native(Runtime* rt, Args&&... args);

    std::string type_name() const { return m_class ? m_class->name() : "Object"; }

    bool is_instance_of(Class& class_) const;

    std::string dump_string() const { std::ostringstream oss; print_impl(oss, true, true); return oss.str(); }
    void repl_print(std::ostream& output, bool print_members) const;

    // These forwards to underlying class
    std::string to_string(Runtime& rt) const;
    Value to_primitive(Runtime&, Value::Type) const;
    Value operator_add(Runtime& rt, Value const& rhs) const;
    CompareResult operator_compare(Runtime& rt, Value const& rhs) const;
    Value operator_subscript(Runtime& rt, Value const& rhs);
    Value call(Runtime&, Object& this_, ArgumentList const&) const;

    template<class T>
    void define_native_function(StringId name, NativeFunctionCallback callback);

protected:
    friend class Class;
    friend class ClassWrapper;

    Object(std::shared_ptr<Class> const& class_);

    std::unordered_map<StringId, std::shared_ptr<MemoryValue>>& members() { return m_members; }
    std::unordered_map<StringId, std::shared_ptr<MemoryValue>> const& members() const { return m_members; }

    // This forwards to underlying class
    void print_impl(std::ostream&, bool print_members, bool dump) const;
    void print_members_impl(std::ostream& output, bool dump) const;

private:
    std::unordered_map<StringId, std::shared_ptr<MemoryValue>> m_members;
    std::shared_ptr<Class> m_class;
};

template<class T>
class NativeObject : public Object
{
public:
    using InternalData = typename std::conditional<requires { typename T::InternalData; }, typename T::InternalData, ObjectInternalData>::type;

    InternalData& internal_data() { return m_internal_data; }
    InternalData const& internal_data() const { return m_internal_data; }

private:
    friend class NativeClass<T>;

    NativeObject(std::shared_ptr<Class> const& class_)
    : Object(class_) {}

    InternalData m_internal_data;
};

template<class T>
std::shared_ptr<NativeObject<T>> NativeClass<T>::create_object(Runtime& rt, ArgumentList const& constructor_args) const
{
    auto object = std::make_shared<NativeObject<T>>(shared_from_this());
    constructor(rt, *object, constructor_args);
    return object;
}

template<class T>
template<class... Args>
std::shared_ptr<NativeObject<T>> NativeClass<T>::create_object(Runtime& rt, Args&&... args) const
{
    auto object = std::make_shared<NativeObject<T>>(shared_from_this());
    constructor(rt, *object, std::forward<Args>(args)...);
    return object;
}

template<class T, class... Args> requires std::is_base_of_v<NativeClass<T>, T>
std::shared_ptr<NativeObject<T>> Object::create_native(Runtime* rt, Args&&... args)
{
    return NativeClass<T>::class_object()->create_object(rt, std::forward<Args>(args)...);
}

template<class T> requires std::is_base_of_v<NativeClass<T>, T>
std::shared_ptr<Object> ClassWrapper::create()
{
    auto class_wrapper = Object::create_native<ClassWrapper>(nullptr, NativeClass<T>::class_object());
    if constexpr(requires(Object& w) { T::init_static_class_members(w); })
        T::init_static_class_members(*class_wrapper);
    return class_wrapper;
}

}
