#pragma once

#include <libevogen/Task.h>
#include <libevogen/Turtle.h>

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace evo
{

class Generator
{
public:
    Generator(Vector<int> position = {})
    : m_turtle(position) {}
    
    template<class T, class... Args>
    void add_task(Args&&... args)
    {
        m_tasks.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    void generate(std::ostream&) const;
    
    void generate_to_stdout() const { generate(std::cout); }
    bool generate_to_file(std::string const&) const;

    Turtle const& turtle() const { return m_turtle; }
    Turtle& turtle() { return m_turtle; }

    std::ostream* stream() const { return m_stream; }

private:
    std::vector<std::unique_ptr<Task>> m_tasks;
    Turtle m_turtle;
    mutable std::ostream* m_stream = nullptr;
};

}
