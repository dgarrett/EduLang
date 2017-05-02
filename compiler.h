#pragma once

#include <stack>
#include "parser.h"

enum class opcode : uint32_t
{
    pushNum,
    pushVar,
    pop,
    add,
    sub,
    mult,
    div,
    mod,
    _and,
    _or,
    eq,
    call,
};

struct Instruction
{
    Instruction() {};
    Instruction(opcode op) : op(op) {};
    Instruction(opcode op, std::vector<int64_t> p) : op(op), p(p) {};
    opcode op;
    std::vector<int64_t> p;
};

struct Variable
{
    Variable() : name(), offset(0) {}
    Variable(std::string name, int64_t offset) : name(name), offset(offset) {}
    std::string name;
    int64_t offset;
};

struct Function
{
    uint64_t addr;
    std::vector<std::string> params;
};

class Compiler
{
public:
    Compiler(Node program);
    std::vector<Instruction> Compile();
    std::string ToString();
    std::pair<std::map<std::string,Function>,std::vector<uint64_t>> Serialize();

private:
    void Compile(Node n);
    void Compile(std::vector<Node> n);
    Variable& FindVar(const std::string& name);

    Node program;
    std::vector<Instruction> iv;
    std::map<std::string, Function> functions;
    std::vector<std::vector<Variable>> varStack;
};
