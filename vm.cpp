#include <iostream>
#include <sstream>
#include <iterator>
#include "vm.h"


vm::vm(std::map<std::string, Function> functions, std::vector<uint64_t> bc)
    : functions(functions), bc(bc)
{}

std::string vm::StackToString()
{
    std::stringstream ss;
    ss << "Stack (size: " << s.size() << ") bottom: ";
    for (auto v : s)
    {
        switch (v.type)
        {
        case svType::num:
            ss << " (num)[" << v.num << "]";
            break;
        case svType::_bool:
            ss << " (bool)[" << (v.b ? "true" : "false") << "]";
            break;
        case svType::stackFrame:
            ss << " (stackFrame)[pc:" << v.sf.returnPc << ",sf:" << v.sf.returnSf << ",numParams:" << v.sf.numParams << "]";
            break;
        default:
            ss << " (unknown type " << (int)v.type << ")";
        }
    }
    return ss.str();
}

sv vm::Run(std::string func, const std::vector<sv>& params)
{
    std::cout << "=======Running " << func << std::endl;
    auto f = functions.at(func);
    bool halt = false;
    if (f.params.size() != params.size()) throw std::exception();
    pc = f.addr;
    sf = 1 + params.size();
    s.insert(s.end(), params.begin(), params.end());
    s.push_back({.type = svType::stackFrame, .sf = {(uint32_t)-1, (uint32_t)-1, (uint32_t)f.params.size()}});
    while (pc < bc.size() && !halt)
    {
        opcode op = (opcode)bc[pc];
        std::cout << StackToString() << std::endl;
        std::cout << "pc: " << pc << " op: " << OpcodeStrings[op] << std::endl;
        pc += 1 + OpcodeParams[op];
        switch(op)
        {
        case opcode::pushNum:
        {
            uint64_t num = bc[pc - 1];
            sv val = {.type = svType::num, .num = *reinterpret_cast<double*>(&num)};
            s.push_back(val);
        }
        break;
        case opcode::getVar:
        {
            uint64_t num = bc[pc - 1];
            s.push_back(s[sf + *reinterpret_cast<int64_t*>(&num)]);
        }
        break;
        case opcode::setVar:
        {
            uint64_t num = bc[pc - 1];
            s[sf + num] = s.back();
            s.pop_back();
        }
        break;
        case opcode::pop:
        {
            s.pop_back();
        }
        break;
        case opcode::add:
        {
            auto b = s.back();
            s.pop_back();
            auto& a = s.back();
            if (a.type != svType::num || b.type != svType::num) throw std::exception();
            s.back() = {svType::num, a.num + b.num};
        }
        break;
        case opcode::sub:
        {
            auto b = s.back();
            s.pop_back();
            auto& a = s.back();
            if (a.type != svType::num || b.type != svType::num) throw std::exception();
            s.back() = {svType::num, a.num - b.num};
        }
        break;
        case opcode::mult:
        {
            auto b = s.back();
            s.pop_back();
            auto& a = s.back();
            if (a.type != svType::num || b.type != svType::num) throw std::exception();
            s.back() = {svType::num, a.num * b.num};
        }
        break;
        case opcode::_div:
        {
            auto b = s.back();
            s.pop_back();
            auto& a = s.back();
            if (a.type != svType::num || b.type != svType::num) throw std::exception();
            s.back() = {svType::num, a.num / b.num};
        }
        break;
        case opcode::eq:
        {
            auto b = s.back();
            s.pop_back();
            auto& a = s.back();
            if (a.type != svType::num || b.type != svType::num) throw std::exception();
            s.back() = {.type = svType::_bool, .b = a.num == b.num};
        }
        break;
        case opcode::call:
        {
            auto f = functions.begin();
            std::advance(f, bc[pc - 1]);
            sv val = {.type = svType::stackFrame, .sf = {pc, sf, (uint32_t)f->second.params.size()}};
            s.push_back(val);
            pc = f->second.addr;
            sf = s.size();
        }
        break;
        case opcode::_return:
        {
            auto r = s.back();
            while (s.size() > sf)
            {
                s.pop_back();
            }
            auto currentSf = s.back();
            s.pop_back(); // stack frame
            // todo pop params
            for (int i = 0; i < currentSf.sf.numParams; ++i)
            {
                s.pop_back();
            }
            s.push_back(r);
            if (currentSf.sf.returnSf == (uint32_t)-1)
            {
                halt = true;
            }
            else
            {
                pc = currentSf.sf.returnPc;
                sf = currentSf.sf.returnSf;
            }
        }
        break;
        case opcode::jf:
        {
            auto a = s.back();
            s.pop_back();
            if (a.type != svType::_bool) throw std::exception();
            if (!a.b)
            {
                pc = bc[pc - 1];
            }
        }
        break;
        case opcode::jt:
        {
            auto a = s.back();
            s.pop_back();
            if (a.type != svType::_bool) throw std::exception();
            if (a.b)
            {
                pc = bc[pc - 1];
            }
        }
        break;
        case opcode::jmp:
        {
            pc = bc[pc - 1];
        }
        break;
        default:
            throw std::exception();
        }
        if (halt)
        {
            std::cout << StackToString() << std::endl;
            std::cout << "pc: " << pc << " op: " << OpcodeStrings[op] << std::endl;
        }
    }

    return s.back();
}
