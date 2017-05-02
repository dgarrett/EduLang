#include <iostream>
#include <sstream>
#include <experimental/optional>
#include "compiler.h"

std::array<std::string, 12> OpcodeStrings
{{
    "pushNum",
    "pushVar",
    "pop",
    "add",
    "sub",
    "mult",
    "div",
    "mod",
    "and",
    "or",
    "eq",
    "call",
}};

Compiler::Compiler(Node program) : program(program)
{}

std::string Compiler::ToString()
{
    std::stringstream ss;
    ss << "====" << std::endl;
    ss << "Functions: " << std::endl;
    for (auto f : functions)
    {
        ss << f.first << " addr: " << f.second.addr << " params:";
        for (auto p : f.second.params)
        {
            ss << " " << p;
        }
        ss << std::endl;
    }
    ss << "====" << std::endl;
    for (int i = 0; i < iv.size(); ++i)
    {
        for (auto f : functions)
        {
            if (f.second.addr == i)
            {
                ss << "//Function: " << f.first << std::endl;
            }
        }
        ss << OpcodeStrings[(size_t)iv[i].op];
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
        for (auto c : n.c)
        {
            std::cout << "Found func: " << c.c[0].tok->text << std::endl;
            functions[c.c[0].tok->text.to_string()];
        }
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
            std::string name = std::string(n.c[0].tok->text);
            std::vector<std::string> params;

            varStack.emplace_back();
            for (int i = 1; i < n.c.size() - 1; ++i)
            {
                params.emplace_back(n.c[i].tok->text);
                varStack[varStack.size() - 1].emplace_back(std::string(n.c[i].tok->text), -i);
            }
            functions[name] = { iv.size(), params };
            Compile(n.c[n.c.size() - 1]);
            varStack.pop_back();
        }
    }
    break;
    case Ident:
    break;
    case Expr:
    case Block:
        //todo
        varStack.emplace_back();
        Compile(n.c);
        varStack.pop_back();
        break;
    case Statement:
    {
        Compile(n.c);
        iv.emplace_back(opcode::pop);
    }
    break;

    case OpAssign:
    {
        auto v = FindVar(n.c[0].tok->text.to_string());
        auto copyC = std::vector<Node>(n.c.begin() + 1, n.c.end());
        Compile(copyC);
    }
    break;
    // Binary ops
    case OpBool:
    case OpEq:
    case OpAdd:
    case OpMult:
    {
        Compile(n.c);
        switch(n.tok->type)
        {
        case TokenType::And:
            iv.emplace_back(opcode::_and);
            break;
        case TokenType::Or:
            iv.emplace_back(opcode::_or);
            break;
        case TokenType::Equal:
            iv.emplace_back(opcode::eq);
            break;
        case TokenType::Add:
            iv.emplace_back(opcode::add);
            break;
        case TokenType::Sub:
            iv.emplace_back(opcode::sub);
            break;
        case TokenType::Mult:
            iv.emplace_back(opcode::mult);
            break;
        case TokenType::Div:
            iv.emplace_back(opcode::div);
            break;
        case TokenType::Mod:
            iv.emplace_back(opcode::mod);
            break;
        default:
            throw std::exception();
        }
    }
    break;
    case OpUnaryPrefix:
    case OpUnaryPostfix:
        //todo
        Compile(n.c);
        break;
    case OpCallLookup:
    {
        auto fi = functions.find(n.c[0].tok->text.to_string());
        if (fi == functions.end())
        {
            throw std::exception();
        }
        std::cout << "Calling " << n.c[0].tok->text << std::endl;
        auto copyC = std::vector<Node>(n.c.rbegin(), n.c.rend() - 1);
        Compile(copyC);
        iv.emplace_back(opcode::call, std::initializer_list<int64_t>{(int64_t)std::distance(functions.begin(), fi)});
    }
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
            Instruction i(opcode::pushNum);
            i.p.push_back(atoi(std::string(n.tok->text).c_str()));
            iv.push_back(i);
        }
        break;
        case TokenType::Ident:
        {
            auto v = FindVar(n.tok->text.to_string());
            iv.emplace_back(opcode::pushVar, std::initializer_list<int64_t>{(int64_t)v.offset});
        }
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

Variable& Compiler::FindVar(const std::string& name)
{
    for (int i = varStack.size() - 1; i >= 0; --i)
    {
        for (int j = 0; j < varStack[i].size(); ++j)
        {
            //std::cout << "varStack variable: " << varStack[i][j].name << std::endl;
            if (varStack[i][j].name == name)
            {
                return varStack[i][j];
            }
        }
    }
    std::cout << "Variable not found in scope stack";
    throw std::exception();
}

std::pair<std::map<std::string,Function>,std::vector<uint64_t>> Compiler::Serialize()
{
    auto bytecode = std::vector<uint64_t>();
    for (auto i : iv)
    {
        bytecode.push_back((uint64_t)i.op);
        for (auto p : i.p)
        {
            bytecode.push_back((uint64_t)p);
        }
    }
    return std::make_pair(functions, std::move(bytecode));
}
