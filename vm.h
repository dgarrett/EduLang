#pragma once
#include <vector>
#include <iostream>
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
    ~hv()
    {
        switch (type)
        {
        case hvType::string:
            delete str;
            break;
        }
    }
    int refCount;
    hvType type;
    union
    {
        std::string* str;
    };
};

struct sf
{
    uint32_t returnPc;
    uint32_t returnSf;
    uint32_t numParams;
};

struct sv
{
    sv(const sv& other)
    {
        *this = other;
    }

    sv& operator=(const sv& other)
    {
        type = other.type;
        switch (type)
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
            //std::cout << "inc copy" << std::endl;
            break;
        }
        return *this;
    }

    ~sv()
    {
        if (type == svType::hv)
        {
            //std::cout << "dec" << std::endl;
            if (0 >= --(heapVal->refCount))
            {
                switch (heapVal->type)
                {
                case hvType::string:
                {
                    delete heapVal;
                    heapVal = nullptr;
                }
                break;
                }
            }
        }
    }
    sv(double num)
    {
        type = svType::num;
        this->num = num;
    }
    sv(bool b)
    {
        type = svType::_bool;
        this->b = b;
    }
    sv(sf sf)
    {
        type = svType::stackFrame;
        this->sf = sf;
    }
    sv(hv* hv)
    {
        type = svType::hv;
        this->heapVal = hv;
        hv->refCount++;
        //std::cout << "inc hv*" << std::endl;
    }

    svType type;
    union
    {
        double num;
        bool b;
        struct sf sf;
        hv* heapVal;
    };
};

class vm
{
public:
    vm(std::map<std::string, Function> functions, std::vector<std::string> stringTable, std::vector<uint64_t> bc);
    sv Run(std::string func, const std::vector<sv>& params);
    void Register(std::string name, std::function<sv(std::vector<sv>&)> func);
private:
    std::string StackToString();

    std::vector<uint64_t> bc;
    std::map<std::string, Function> functions;
    std::vector<sv> s;
    std::vector<hv*> strTable;
    uint32_t pc;
    uint32_t sf;
};
