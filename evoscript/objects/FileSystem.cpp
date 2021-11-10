
#include <evoscript/objects/FileSystem.h>

#include <evoscript/Runtime.h>
#include <evoscript/objects/Array.h>
#include <evoscript/objects/String.h>
#include <evoscript/objects/NativeFunction.h>

#include <cstring>
#include <dirent.h>

namespace evo::script
{

FileSystem::FileSystem()
: Class("FileSystem")
{
    define_native_function<FileSystem>("list_files", list_files);
}

Value FileSystem::list_files(Runtime& rt, Object&, ArgumentList const& args)
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

    auto path_arg = args.get(0);
    std::string path = path_arg.is_undefined() ? working_directory() : path_arg.to_string();

    DIR* dir = opendir(path.c_str());
    if(!dir)
    {
        rt.throw_exception<Exception>("Failed to open directory: " + std::string(strerror(errno)));
        return {};
    }

    std::vector<Value> result;
    while(auto* de = readdir(dir))
    {
        if(!de)
        {
            rt.throw_exception<Exception>("Failed to read directory: " + std::string(strerror(errno)));
            return {};
        }
        result.push_back(Value::new_object(Object::create_native<String>(&rt, de->d_name)));
    }

    closedir(dir);
    return Value::new_object(Array::from_vector(result));
}

}
