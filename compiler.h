#pragma once

#include <stack>
#include "parser.h"

enum opcode : uint32_t
{
    pushNum,
    pushStr,
    getVar,
    setVar,
    pop,
    add,
    sub,
    mult,
    _div,
    mod,
    _and,
    _or,
    eq,
    call,
    _return,
    jf,
    jt,
    jmp,
};

extern std::array<std::string, 18> OpcodeStrings;
extern std::array<int, 18> OpcodeParams;

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

struct sv;

struct Function
{
    uint64_t addr;
    std::vector<std::string> params;
    bool isNative;
    std::function<sv(std::vector<sv>&)> nativeFunc;
};

class Compiler
{
public:
    Compiler(Node program);
    std::vector<uint64_t> Compile();
    std::string ToString();
    std::tuple<std::map<std::string,Function>,std::vector<std::string>,std::vector<uint64_t>> Serialize();

private:
    void Compile(Node n);
    void Compile(std::vector<Node> n);
    Variable& FindVar(const std::string& name);
    void FindVarDecls(const Node& n);

    Node program;
    std::vector<uint64_t> iv;
    std::map<std::string, Function> functions;
    std::vector<std::vector<Variable>> varStack;
    std::map<std::string, int> varOffsets;
    int currVarOffset;
    std::vector<std::string> strTable;
};
