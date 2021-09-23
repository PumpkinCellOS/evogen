#include <evoscript/objects/GlobalObject.h>

#include <evoscript/NativeFunction.h>
#include <evoscript/objects/Array.h>
#include <evoscript/objects/Class.h>
#include <evoscript/objects/Exception.h>
#include <evoscript/objects/FsObject.h>
#include <evoscript/objects/StringObject.h>
#include <evoscript/objects/TimeObject.h>
#include <fstream>

namespace evo::script
{

GlobalObject::GlobalObject()
{
    DEFINE_NATIVE_OBJECT(object, "sys", std::make_shared<SysObject>());
    DEFINE_NATIVE_OBJECT(object, "fs", std::make_shared<FsObject>());

    DEFINE_NATIVE_FUNCTION(GlobalObject, "run", &GlobalObject::run_script);

    DEFINE_NATIVE_OBJECT(object, "Array", create_native_class<Array>());
    DEFINE_NATIVE_OBJECT(object, "Exception", create_native_class<Exception>());
    DEFINE_NATIVE_OBJECT(object, "Object", create_native_class<Object>());
    DEFINE_NATIVE_OBJECT(object, "String", create_native_class<StringObject>());
    DEFINE_NATIVE_OBJECT(object, "Time", create_native_class<TimeObject>());
}

Value GlobalObject::run_script(Runtime& rt, std::vector<Value> const& args)
{
    if(args.size() != 1)
    {
        rt.throw_exception("You need to specify file name");
        return {};
    }
    auto file_name = args[0].to_string();
    std::ifstream file(file_name);
    if(file.fail())
    {
        rt.throw_exception("Failed to open file '" + file_name + "' for executing");
        return {};
    }
    return rt.run_code_from_stream(file, Runtime::RunType::Include);
}

}
