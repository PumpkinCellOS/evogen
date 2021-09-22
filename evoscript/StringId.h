#pragma once

#include <cstddef>
#include <string>

namespace evo::script
{

class StringId
{
public:
    StringId() = default;

    template<size_t S>
    StringId(char const(&id)[S])
    : StringId(std::string_view{id}) {}

    StringId(std::string const& id)
    : StringId(std::string_view{id}) {}

    StringId(std::string_view id);

    std::string string() const;
    operator std::string() const { return string(); }
    bool empty() const { return m_id == 0; }
    size_t id() const { return m_id; }

    bool operator==(StringId const& other) const { return m_id == other.m_id; }
    bool operator!=(StringId const& other) const { return m_id == other.m_id; }

private:
    size_t m_id = 0;
};

}

namespace std
{

template<>
class hash<evo::script::StringId> : public std::hash<size_t>
{
public:
    size_t operator()(evo::script::StringId const& id) const
    {
        return id.id();
    }
};

}
