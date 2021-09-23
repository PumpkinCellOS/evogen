#include "evoscript/NativeFunction.h"
#include <evoscript/objects/FsObject.h>

#include <evoscript/Runtime.h>
#include <evoscript/objects/Array.h>
#include <evoscript/objects/StringObject.h>

#include <cstring>
#include <dirent.h>

namespace evo::script
{

FsObject::FsObject()
{
    define_native_function<FsObject>("list_files", &FsObject::list_files);
}

Value FsObject::list_files(Runtime& rt, std::vector<Value> const& args) const
{
    if(args.size() < 1)
    {
        rt.throw_exception("Missing argument: path");
        return {};
    }
    auto path = args[0].to_string();

    DIR* dir = opendir(path.c_str());
    if(!dir)
    {
        rt.throw_exception("Failed to open directory: " + std::string(strerror(errno)));
        return {};
    }

    std::vector<Value> result;
    while(auto* de = readdir(dir))
    {
        if(!de)
        {
            rt.throw_exception("Failed to read directory: " + std::string(strerror(errno)));
            return {};
        }
        result.push_back(new_object_value<StringObject>(de->d_name));
    }

    closedir(dir);
    return Value::new_object(Array::from_std_vector(result));
}

}
