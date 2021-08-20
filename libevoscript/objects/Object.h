#pragma once

#include <libevoscript/Value.h>

namespace evo::script
{

class Runtime;
class MemoryValue;

class Object
{
public:
    virtual Value get(std::string const& member) = 0;
    virtual std::string type_name() const { return "Object"; }
    virtual std::string dump_string() const { return ""; }
    virtual Value call(Runtime&, Object& container, std::vector<Value> const& arguments);
};

// TODO: Move these to separate files
class MapObject : public Object
{
public:
    virtual Value get(std::string const& member) override;
    virtual std::string type_name() const override { return "MapObject"; }
    virtual std::string dump_string() const override;

private:
    std::map<std::string, std::shared_ptr<MemoryValue>> m_values;
};

class Function : public Object
{
public:
    virtual Value get(std::string const& member) override;
    virtual std::string type_name() const override { return "Function"; }
    virtual std::string dump_string() const override { return "function() {}"; }
};

}
