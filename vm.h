#pragma once
#include <vector>
#include "compiler.h"

enum class svType : uint32_t
{
    num
};

struct sv
{
    svType type;
    union
    {
        double num;
    };
};

class vm
{
public:
    vm(std::map<std::string, Function> functions, std::vector<uint64_t> bc);
    void Run(std::string func);
private:
    std::vector<uint64_t> bc;
    std::map<std::string, Function> functions;
    std::vector<sv> s;
    int pc;
};
