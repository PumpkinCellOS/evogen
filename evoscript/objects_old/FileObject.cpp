#include <evoscript/objects/FileObject.h>

#include <evoscript/NativeFunction.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/Object.h>
#include <evoscript/objects/StringObject.h>

#include <cstring>

namespace evo::script
{

FileObject::FileObject(Runtime& rt, std::string const& name)
: m_file(name)
{
    static StringId read_sid = "read";
    define_native_function<FileObject>(read_sid, &FileObject::read);
    static StringId close_sid = "close";
    define_native_function<FileObject>(close_sid, &FileObject::close);
    static StringId eof_sid = "eof";
    define_native_function<FileObject>(eof_sid, &FileObject::eof);

    if(name.empty())
        rt.throw_exception("`name` must not be empty");
    if(m_file.fail())
        rt.throw_exception("Failed to open file '" + name + "': " + std::string{strerror(errno)});
}

Value FileObject::read(Runtime& rt, ArgumentList const& args)
{
    int bytes = args.get(0).to_int(rt);
    if(rt.has_exception())
        return {};
    if(bytes < 0)
    {
        rt.throw_exception("`bytes` must not be less than 0");
        return {};
    }

    std::string data;
    data.resize(bytes);
    if(!(m_file.read(data.data(), bytes)))
        return Value::null();
    return new_object_value<StringObject>(data);
}

Value FileObject::close(Runtime&, ArgumentList const&)
{
    m_file.close();
    return Value::undefined();
}

Value FileObject::eof(Runtime&, ArgumentList const&)
{
    return Value::new_bool(m_file.eof());
}

}
