#include <evoscript/Object.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/Exception.h>
#include <evoscript/objects/NativeFunction.h>
#include <fstream>

using namespace evo::script;

class TestObject : public GlobalObject
{
public:
    TestObject()
    {
        define_native_function<Class>("test_fail", test_fail);
        define_native_function<Class>("throws", throws);
    }

private:
    // TODO: Remove it once a try-catch construction is implemented
    static Value throws(Runtime& rt, Object&, ArgumentList const& args)
    {
        args.get(0).call(rt, ArgumentList{});
        bool has_exception = rt.has_exception();
        rt.clear_exception();
        return Value::new_bool(has_exception);
    }
    static Value test_fail(Runtime& rt, Object&, ArgumentList const& args)
    {
        std::string test_name = args.is_given(0) ? args.get(0).to_string() : "";
        rt.throw_exception<Exception>("Test failed: " + test_name);
        return {};
    }
};

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cout << "Usage: TestRunner [name]" << std::endl;
        return 2;
    }
    std::string_view test_name = argv[1];
    Runtime rt(std::make_shared<TestObject>());
    std::ifstream library("test-lib.evs");
    rt.run_code_from_stream(library, Runtime::RunType::Include);
    if(rt.has_exception())
    {
        std::cout << "\e[33mFailed to load test library\e[0m:" << std::endl;
        rt.exception()->repl_print(std::cout, true);
        return 2;
    }
    std::ifstream test(std::string{test_name});
    auto result = rt.run_code_from_stream(test, Runtime::RunType::Script);
    if(result.is_invalid())
    {
        std::cout << "---- \e[31mFAILED\e[0m: " << test_name << std::endl;
        return 1;
    }
    std::cout << "---- \e[32mPASSED\e[0m: " << test_name << std::endl;
    return 0;
}
