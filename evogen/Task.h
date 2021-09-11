#pragma once

#include <evogen/Block.h>
#include <evogen/Turtle.h>

#include <ostream>

namespace evo
{

class Generator;

class Task
{
public:
    virtual void generate_code(Generator const&) const = 0;
};

class PlaceBlockTask : public Task
{
public:
    PlaceBlockTask(Block const& block, BlockPosition const& position)
    : m_block(block), m_position(position) {}

    virtual void generate_code(Generator const&) const override;

private:
    Block m_block;
    BlockPosition m_position;
};

class FillBlocksTask : public Task
{
public:
    FillBlocksTask(Block const& block, BlockPosition const& start_position, BlockPosition const& end_position)
    : m_block(block), m_start_position(start_position), m_end_position(end_position) {}

    virtual void generate_code(Generator const&) const override;

private:
    Block m_block;
    BlockPosition m_start_position;
    BlockPosition m_end_position;
};

class MoveTurtleTask : public Task
{
public:
    MoveTurtleTask(BlockPosition const& position, Turtle& turtle)
    : m_position(position), m_turtle(turtle) {}

    virtual void generate_code(Generator const&) const override;

private:
    BlockPosition m_position;
    Turtle& m_turtle;
};

}
