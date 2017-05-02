#include <iostream>
#include "vm.h"


vm::vm(std::map<std::string, Function> functions, std::vector<uint64_t> bc)
    : functions(functions), bc(bc)
{}

void vm::Run(std::string func)
{
    auto f = functions.at(func);
    pc = f.addr;
    while (pc < bc.size())
    {
        opcode op = (opcode)bc[pc];
        std::cout << "pc: " << pc << " op: " << (int)op << std::endl;
        switch(op)
        {
        case opcode::pushNum:
        {
            double num = (double)bc[pc + 1];
            s.push_back({svType::num, num});
            pc++;
        }
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
        default:
            throw std::exception();
        }
        pc++;
    }
}
