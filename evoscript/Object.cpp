#include <evoscript/Object.h>

#include <evoscript/AbstractOperations.h>
#include <evoscript/EscapeSequences.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/Exception.h>
#include <evoscript/objects/NativeFunction.h>

namespace evo::script
{

ClassWrapper::ClassWrapper()
: Class("ClassWrapper")
{
    define_native_function<ClassWrapper>("__construct", [](Runtime& rt, Object& t, ArgumentList const& args) {
        return Value::new_object(Object::create(rt, t.internal_data<InternalData>().underlying_class, args));
    });
}

void ClassWrapper::print(Object const& object, std::ostream& out, bool detailed, bool dump) const
{
    if(dump)
    {
        Class::print(object, out, detailed, dump);
        return;
    }
    auto const& class_ = object.internal_data<InternalData>().underlying_class;
    out << escapes::keyword("class") << " " << escapes::type(class_->name()) << " ";
    if(detailed)
        object.print_members_impl(out, false);
    else
        out << "{...}";
}

std::shared_ptr<MemoryValue> Class::resolve_class_member(StringId member) const
{
    auto result = m_vtable.find(member);
    if(result != m_vtable.end())
    {
        auto memval = MemoryValue::create_object(result->second);
        memval->set_name(member);
        return memval;
    }
    if(!m_base.expired())
        return m_base.lock()->resolve_class_member(member);
    return nullptr;
}

bool Class::is_same_or_base_of(Class& class_) const
{
    return this == &class_ || (!m_base.expired() && m_base.lock()->is_same_or_base_of(class_));
}

void Class::define_virtual_member(StringId name, std::shared_ptr<Object> const& value)
{
    m_vtable.insert({name, value});
}

Value Class::operator_add(Runtime& rt, Object const&, Value const& rhs) const
{
    rt.throw_exception<Exception>("Add operator not defined for class");
    return {};
}

// function operator<>(rhs: Value) : CompareResult
CompareResult Class::operator_compare(Runtime& rt, Object const&, Value const& rhs) const
{
    rt.throw_exception<Exception>("Compare operator not defined for class");
    return CompareResult::Unknown;
}

// function operator[](rhs: Value) : Value
Value Class::operator_subscript(Runtime& rt, Object& object, Value const& rhs) const
{
    return object.get(rhs.to_string())->value();
}

void Class::print(Object const& object, std::ostream& output, bool detailed, bool dump) const
{
    using namespace escapes;
    output << type(name()) << " ";
    if(detailed)
        object.print_members_impl(output, dump);
    else
        output << "{...}";
}

StringId Class::to_string_sid =    "@to_string";
StringId Class::to_primitive_sid = "@to_primitive";
StringId Class::op_add_sid =       "@+add";
StringId Class::op_compare_sid =   "@+compare";
StringId Class::op_subscript_sid = "@+subscript";
StringId Class::print_sid =        "@print";

Object::Object(std::shared_ptr<Class> const& class_)
: m_class(class_) {}

Object::~Object()
{
    if(m_class)
        m_class->destructor(*this);
}

std::shared_ptr<Object> Object::create(Runtime& rt, std::shared_ptr<Class> const& class_, ArgumentList const& args)
{
    return class_->create_object(rt, args);
}

std::shared_ptr<MemoryValue> Object::get(StringId member)
{
    if(m_class)
    {
        auto result = m_class->resolve_class_member(member);
        if(result)
            return result;
    }
    
    auto it = m_members.find(member);
    if(it == m_members.end())
    {
        // Reference to newly-created undefined.
        // TODO: Tidy up these undefineds after exiting an expression
        return m_members.insert(std::make_pair(member, MemoryValue::create_undefined())).first->second;
    }

    return it->second;
}

std::shared_ptr<MemoryValue> Object::get_without_side_effects(StringId member) const
{
    if(m_class)
    {
        auto result = m_class->resolve_class_member(member);
        if(result)
            return result;
    }

    auto it = m_members.find(member);
    if(it == m_members.end())
        return nullptr;
    return it->second;
}

bool Object::is_instance_of(Class& class_) const
{
    return m_class ? m_class->is_same_or_base_of(class_) : (&class_ == NativeClass<Class>::class_object().get());
}

std::string Object::to_string(Runtime& rt) const
{
    return get_without_side_effects(Class::to_string_sid).
}

Value Object::to_primitive(Runtime& rt, Value::Type type) const
{
    if(!m_class)
    {
        rt.throw_exception<Exception>("Cannot convert " + type_name() + " to primitive");
        return {};
    }
    return m_class->to_primitive(rt, *this, type);
}

Value Object::operator_add(Runtime& rt, Value const& rhs) const
{
    if(!m_class)
    {
        rt.throw_exception<Exception>("Cannot call add operator without class");
        return {};
    }
    return m_class->operator_add(rt, *this, rhs);
}

CompareResult Object::operator_compare(Runtime& rt, Value const& rhs) const
{
    if(!m_class)
    {
        rt.throw_exception<Exception>("Cannot call compare operator without class");
        return {};
    }
    return m_class->operator_compare(rt, *this, rhs);
}

Value Object::operator_subscript(Runtime& rt, Value const& rhs)
{
    if(!m_class)
        return get(rhs.to_string())->value();
    return m_class->operator_subscript(rt, *this, rhs);
}

Value Object::call(Runtime& rt, Object& this_, ArgumentList const& args) const
{
    if(!m_class)
    {
        rt.throw_exception<Exception>("Cannot call object without class");
        return {};
    }
    return m_class->call(rt, *this, this_, args);
}

void Object::repl_print(std::ostream& output, bool print_members) const
{
    print_impl(output, print_members, false);
}

void Object::print_impl(std::ostream& output, bool print_members, bool dump) const
{
    if(m_class)
    {
        m_class->print(*this, output, print_members, dump);
        return;
    }
    
    using namespace escapes;
    output << type(type_name()) << " ";
    if(print_members)
        print_members_impl(output, dump);
    else
        output << "{...}";
}

void Object::print_members_impl(std::ostream& output, bool dump) const
{
    using namespace escapes;
    output << "{";
    if(!m_members.empty() || (m_class && !m_class->m_vtable.empty()))
        output << std::endl;

    {
        size_t counter = 0;
        for(auto& pair: m_members)
        {
            auto id_string = pair.first.string();
            auto const& value = pair.second->value();
            if(value.is_object() && value.get_object().get() == this)
                output << "  " << literal(id_string) << ": " << constant("<recursive reference>");
            else
            {
                output << "  " << literal(id_string) << ": ";
                if(dump)
                    output << pair.second->dump_string();
                else
                    pair.second->repl_print(output, false);
            }
            if(counter < m_members.size() - 1)
                output << std::endl;
            counter++;
        }
        if(counter > 0)
            output << std::endl;
    }

    if(m_class)
    {
        size_t counter = 0;
        for(auto& pair: m_class->m_vtable)
        {
            auto id_string = pair.first.string();
            output << "  " << literal(id_string) << "#: ";
            if(dump)
                output << pair.second->dump_string();
            else
                pair.second->repl_print(output, false);
            if(counter < m_class->m_vtable.size() - 1)
                output << std::endl;
            counter++;
        }
           
        if(counter > 0)
            output << std::endl;
    }
    
    output << "}";
}

}
