#pragma once
#include <vector>
#include "compiler.h"

enum class svType : uint32_t
{
    num,
    _bool,
    stackFrame,
    hv
};

enum class hvType : uint32_t
{
    string
};

struct hv
{
    int refCount;
    hvType type;
    union
    {
        std::string* str;
    };
};

struct sv
{
    /*sv() = default;
    sv(sv& other)
    {
        switch (other.type)
        {
        case svType::num:
            num = other.num;
            break;
        case svType::_bool:
            b = other.b;
            break;
        case svType::stackFrame:
            sf = other.sf;
            break;
        case svType::hv:
            heapVal = other.heapVal;
            heapVal->refCount++;
            break;
        }
    }
    ~sv()
    {
        if (type == svType::hv)
        {
            if (0 >= --(heapVal->refCount))
            {
                switch (heapVal->type)
                {
                case hvType::string:
                {
                    delete heapVal->str;
                }
                break;
                }
            }
        }
        }*/
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
        hv* heapVal;
    };
};

class vm
{
public:
    vm(std::map<std::string, Function> functions, std::vector<std::string> stringTable, std::vector<uint64_t> bc);
    sv Run(std::string func, const std::vector<sv>& params);
private:
    std::string StackToString();

    std::vector<uint64_t> bc;
    std::map<std::string, Function> functions;
    std::vector<sv> s;
    std::vector<hv*> strTable;
    uint32_t pc;
    uint32_t sf;
};
