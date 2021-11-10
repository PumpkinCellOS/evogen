#include <evoscript/GlobalObject.h>

#include <evoscript/Object.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/Array.h>
#include <evoscript/objects/Exception.h>
#include <evoscript/objects/FileSystem.h>
#include <evoscript/objects/NativeFunction.h>
#include <evoscript/objects/String.h>
#include <evoscript/objects/System.h>
#include <evoscript/objects/Time.h>
#include <fstream>

namespace evo::script
{

GlobalObject::GlobalObject()
: ScopeObject(nullptr)
{
    allocate("Array")->value() = Value::new_object(ClassWrapper::create<Array>());
    allocate("Exception")->value() = Value::new_object(ClassWrapper::create<Exception>());
    allocate("Object")->value() = Value::new_object(ClassWrapper::create<Class>());
    allocate("String")->value() = Value::new_object(ClassWrapper::create<String>());
    allocate("Time")->value() = Value::new_object(ClassWrapper::create<Time>());

    allocate("fs")->value() = Value::new_object(Object::create_native<FileSystem>(nullptr));
    allocate("sys")->value() = Value::new_object(Object::create_native<System>(nullptr));

    allocate("run")->value() = Value::new_object(Object::create_native<NativeFunction<Class>>(nullptr, run_script));
    allocate("to_int")->value() = Value::new_object(Object::create_native<NativeFunction<Class>>(nullptr, [](Runtime& rt, Object&, ArgumentList const& args) {
        return Value::new_int(args.get(0).to_int(rt));
    }));
    allocate("to_bool")->value() = Value::new_object(Object::create_native<NativeFunction<Class>>(nullptr, [](Runtime& rt, Object&, ArgumentList const& args) {
        return Value::new_bool(args.get(0).to_bool(rt));
    }));
}

Value GlobalObject::run_script(Runtime& rt, Object&, ArgumentList const& args)
{
    auto file_name_arg = args.get(0);
    if(file_name_arg.is_undefined())
    {
        rt.throw_exception<Exception>("You need to specify file name");
        return {};
    }
    auto file_name = file_name_arg.to_string();
    std::ifstream file(file_name);
    if(file.fail())
    {
        rt.throw_exception<Exception>("Failed to open file '" + file_name + "' for executing");
        return {};
    }
    return rt.run_code_from_stream(file, Runtime::RunType::Include);
}

}
