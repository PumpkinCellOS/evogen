#include <evoscript/objects/File.h>

#include <evoscript/Runtime.h>
#include <evoscript/objects/NativeFunction.h>
#include <evoscript/objects/String.h>

#include <cstring>

namespace evo::script
{

File::File()
: Class("File")
{
    define_native_function<File>("read", read);
    define_native_function<File>("close", close);
    define_native_function<File>("eof", eof);
}

void constructor(Runtime& rt, NativeObject<File>& object, ArgumentList const& args) const
{
    constructor(rt, object, args.get_or(0, Value::new_object(Object::create_native<String>(rt, ""))).to_string());
}

void constructor(Runtime&, NativeObject<File>&, std::string const& file_name) const;

std::unique_ptr<ObjectInternalData> File::construct_internal_data(Runtime* rt, ArgumentList const& args) const
{
}

std::unique_ptr<ObjectInternalData> File::construct_internal_data(Runtime* rt, std::string const& file_name) const
{
    assert(rt);
    auto internal_data = std::make_unique<InternalData>();
    if(file_name.empty())
    {
        rt->throw_exception<Exception>("`name` must not be empty");
        return nullptr;
    }
    internal_data->file.open(file_name);
    if(internal_data->file.fail())
        rt->throw_exception<Exception>("Failed to open file '" + file_name + "': " + std::string{strerror(errno)});
    return internal_data;
}

Value File::read(Runtime& rt, Object& object, ArgumentList const& args)
{
    auto& file = object.internal_data<InternalData>().file;
    int bytes = args.get(0).to_int(rt);
    if(rt.has_exception())
        return {};
    if(bytes < 0)
    {
        rt.throw_exception<Exception>("`bytes` must not be less than 0");
        return {};
    }

    std::string data;
    data.resize(bytes);
    if(!(file.read(data.data(), bytes)))
        return Value::null();
    return Value::new_object(Object::create_native<String>(&rt, data));
}

Value File::close(Runtime&, Object& object, ArgumentList const&)
{
    auto& file = object.internal_data<InternalData>().file;
    file.close();
    return Value::undefined();
}

Value File::eof(Runtime&, Object& object, ArgumentList const&)
{
    auto& file = object.internal_data<InternalData>().file;
    return Value::new_bool(file.eof());
}

}
