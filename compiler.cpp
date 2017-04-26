#include <iostream>
#include <sstream>
#include "compiler.h"

std::array<std::string, 4> OpcodeStrings
{{
    "push",
    "pop",
    "add",
    "sub",
}};

Compiler::Compiler(Node program) : program(program)
{}

std::string Compiler::ToString()
{
    std::stringstream ss;
    for (int i = 0; i < iv.size(); ++i)
    {
        for (auto f : functions)
        {
            if (f.second.addr == i)
            {
                std::cout << "//Function: " << f.first << std::endl;
            }
        }
        ss << OpcodeStrings[iv[i].op];
        for (auto p : iv[i].p)
        {
            ss << " " << p;
        }
        ss << std::endl;
    }

    return ss.str();
}

std::vector<Instruction> Compiler::Compile()
{
    Compile(program);
    return iv;
}

void Compiler::Compile(std::vector<Node> nv)
{
    for (auto n : nv)
    {
        Compile(n);
    }
}

void Compiler::Compile(Node n)
{
    std::cout << "Compiling NodeType: " << NodeTypeStrings[n.t];
    if (n.tok)
    {
        std::cout << " Token: \"" << n.tok->text << "\" (" << TokenName(n.tok->type) << ")";
    }
    std::cout << std::endl;

    switch (n.t)
    {
    case Program:
        Compile(n.c);
        break;
    case Decl:
    {
        if (n.tok->type == TokenType::Equal)
        {
            //
        }
        else if (n.tok->type == TokenType::Colon)
        {
            functions[std::string(n.c[0].tok->text)] = { iv.size(), n.c.size()-2 };
            Compile(n.c[n.c.size() - 1]);
        }
    }
    break;
    case Ident:
        break;
    case Expr:
    case Block:
        //todo
        Compile(n.c);
        break;
    case Statement:
    {
        Compile(n.c);
        iv.emplace_back(opcode::pop);
    }
    break;

    case OpAssign:
    case OpBool:
    case OpEq:
        //todo
        Compile(n.c);
        break;
    case OpAdd:
    {
        Compile(n.c);
        switch(n.tok->type)
        {
        case TokenType::Add:
            iv.emplace_back(opcode::add);
            break;
        case TokenType::Sub:
            iv.emplace_back(opcode::sub);
            break;
        default:
            throw std::exception();
        }
    }
    break;
    case OpMult:
    case OpUnary:
    case OpCallLookup:
        //todo
        Compile(n.c);
        break;
    case OpEnd:
    {
        switch(n.tok->type)
        {
        case TokenType::LParen:
        {
            Compile(n.c);
        }
        break;
        case TokenType::Number:
        {
            Instruction i(opcode::push);
            i.p.push_back(atoi(std::string(n.tok->text).c_str()));
            iv.push_back(i);
        }
        break;
        case TokenType::Ident:
            break;
        default:
            throw std::exception();
        }
    }
    break;
    default:
        throw std::exception();
        break;
    }
}
