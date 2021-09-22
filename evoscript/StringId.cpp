#include <evoscript/StringId.h>

#include <cassert>
#include <iostream>
#include <unordered_map>

namespace evo::script
{

static std::unordered_map<std::string, size_t> s_string_to_id_registry;
static std::unordered_map<size_t, std::string> s_id_to_string_registry;
static size_t s_last_string_id = 0;

StringId::StringId(std::string_view string)
{
    if(string.empty())
    {
        //std::cout << "Reused empty StringID" << std::endl;
        return;
    }

    auto id = s_string_to_id_registry.find(std::string{string});
    if(id == s_string_to_id_registry.end())
    {
        m_id = s_string_to_id_registry.insert(std::make_pair(string, ++s_last_string_id)).first->second;
        s_id_to_string_registry.insert(std::make_pair(s_last_string_id, string));
        //std::cout << "Generated new StringId: " << string << "[" << string.size() << "]" << ": " << m_id << std::endl;
    }
    else
    {
        m_id = id->second;
        //std::cout << "Reused StringId: " << string << "[" << string.size() << "]" << ": " << m_id << std::endl;
    }
}

std::string StringId::string() const
{
    if(m_id == 0)
        return "";
    auto string = s_id_to_string_registry.find(m_id);
    assert(string != s_id_to_string_registry.end());
    return string->second;
}

}
