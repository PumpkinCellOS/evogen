#pragma once

#include <libevoscript/objects/Object.h>
#include <map>

namespace evo::script
{

class MapObject : public Object
{
public:
    virtual Value get(std::string const& member) override;
    virtual std::string type_name() const override { return "MapObject"; }
    virtual std::string repl_string() const override;

private:
    std::map<std::string, std::shared_ptr<MemoryValue>> m_values;
};

}
