#pragma once

#include "parser.h"

enum opcode : uint32_t
{
    push,
    pop,
    add,
    sub,
};

struct Instruction
{
    Instruction() {};
    Instruction(opcode op) : op(op) {};
    opcode op;
    std::vector<uint32_t> p;
};

struct Function
{
    uint64_t addr;
    uint64_t params;
};

class Compiler
{
public:
    Compiler(Node program);
    std::vector<Instruction> Compile();
    std::string ToString();

private:
    void Compile(Node n);
    void Compile(std::vector<Node> n);

    Node program;
    std::vector<Instruction> iv;
    std::map<std::string, Function> functions;
};
