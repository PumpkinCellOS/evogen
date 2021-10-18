#include <evoscript/Runtime.h>
#include <evoscript/NativeFunction.h>
#include <evoscript/objects/Exception.h>
#include <fstream>

using namespace evo::script;

class TestObject : public GlobalObject
{
public:
    TestObject()
    {
        static StringId test_fail_sid = "test_fail";
        define_native_function<TestObject>(test_fail_sid, &TestObject::test_fail);
        static StringId throws_sid = "throws";
        define_native_function<TestObject>(throws_sid, &TestObject::throws);
    }

private:
    // TODO: Remove it once a try-catch construction is implemented
    Value throws(Runtime& rt, std::vector<Value> const& args)
    {
        Value function = args.size() < 1 ? Value::undefined() : args[0];
        function.call(rt, {});
        bool has_exception = rt.has_exception();
        rt.clear_exception();
        return Value::new_bool(has_exception);
    }
    Value test_fail(Runtime& rt, std::vector<Value> const& args)
    {
        std::string test_name = args.size() < 1 ? "" : args[0].to_string();
        rt.throw_exception("Test failed (should throw): " + test_name);
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
    rt.run_code_from_stream(test, Runtime::RunType::Script);
    if(rt.has_exception())
    {
        std::cout << "---- \e[31mFAILED\e[0m: " << test_name << std::endl;
        rt.exception()->repl_print(std::cout, true);
        return 1;
    }
    std::cout << "---- \e[32mPASSED\e[0m: " << test_name << std::endl;
    return 0;
}
