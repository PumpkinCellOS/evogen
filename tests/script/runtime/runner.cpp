#include <evoscript/objects/Exception.h>
#include <evoscript/Runtime.h>
#include <exception>
#include <fstream>
#include <iostream>

using namespace evo::script;

struct TestResult
{
    std::string name;
    Value expected_value;
    enum CompletionType {
        Throw,
        Normal
    } type = Normal;
};

int main()
{
    // This file should be run from 'tests/script' directory
    // TODO: Auto-detect it
    TestResult tests[] = {
        {"loops/while.evs", Value::new_int(10)},
        {"scopes/block.evs", Value::undefined()}
    };

    bool test_failed = false;

    for(auto test: tests)
    {
        Runtime runtime;
        std::ifstream file {"lang/" + test.name};
        if(file.fail())
        {
            std::cout << test.name << ": FAIL: Failed to open file" << std::endl;
            test_failed = true;
            continue;
        }
        auto actual_value = runtime.run_code_from_stream(file, Runtime::RunType::Include).dereferenced();
        if(runtime.has_exception() && test.type != TestResult::Throw)
        {
            std::cout << test.name << ": FAIL: Test has thrown ";
            runtime.exception()->repl_print(std::cout, false);
            std::cout << std::endl;
            test_failed = true;
            continue;
        }
        if(test.type == TestResult::Throw)
        {
            if(!runtime.has_exception())
            {
                std::cout << test.name << ": FAIL: Test has not thrown, expected ";
                test.expected_value.repl_print(std::cout, false);
                std::cout << " to be thrown" << std::endl;
            }
            else if(!actual_value.is_object() || runtime.exception() != actual_value.get_object())
            {
                std::cout << test.name << ": FAIL: Test has thrown ";
                runtime.exception()->repl_print(std::cout, false);
                std::cout << ", expected ";
                test.expected_value.repl_print(std::cout, false);
                actual_value.repl_print(std::cout, false);
                std::cout << " to be thrown" << std::endl;
            }
            test_failed = true;
            continue;
        }
        if(actual_value == test.expected_value)
            std::cout << test.name << ": PASS" << std::endl;
        else
        {
            std::cout << test.name << ": FAIL: invalid value, expected ";
            test.expected_value.repl_print(std::cout, false);
            std::cout << ", got ";
            actual_value.repl_print(std::cout, false);
            std::cout << std::endl;
            test_failed = true;
        }
    }
    return test_failed ? 1 : 0;
}
