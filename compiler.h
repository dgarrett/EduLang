#pragma once

#include "parser.h"

enum opcode : uint32_t
{
    push,
    add,
};

struct Instruction
{
    opcode op;
    std::vector<uint32_t> p;
};

class Compiler
{
public:
    Compiler(Node program);
    void Compile(Node n);
    void Compile(std::vector<Node> n);

private:
    std::vector<Instruction> iv;
};
