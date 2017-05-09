#pragma once
#include <vector>
#include "compiler.h"

enum class svType : uint32_t
{
    num,
    _bool,
    stackFrame
};

struct sv
{
    svType type;
    union
    {
        double num;
        bool b;
        struct {
            uint32_t returnPc;
            uint32_t returnSf;
            uint32_t numParams;
        } sf;
    };
};

class vm
{
public:
    vm(std::map<std::string, Function> functions, std::vector<uint64_t> bc);
    sv Run(std::string func, const std::vector<sv>& params);
private:
    std::string StackToString();

    std::vector<uint64_t> bc;
    std::map<std::string, Function> functions;
    std::vector<sv> s;
    uint32_t pc;
    uint32_t sf;
};
