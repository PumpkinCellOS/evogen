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
    auto working_directory = []() {
        #ifdef __unix__
            char* buffer = getcwd(nullptr, 0);
            std::string result(buffer);
            free(buffer);
            return result;
        #else
            return ".";
        #endif
    };

    std::string path = args.size() == 0 ? working_directory() : args[0].to_string();

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
