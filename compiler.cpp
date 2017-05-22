#include <iostream>
#include <sstream>
#include <experimental/optional>
#include "compiler.h"

std::array<std::string, 18> OpcodeStrings
{{
    "pushNum",
    "pushStr",
    "getVar",
    "setVar",
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
    "return",
    "jf",
    "jt",
    "jmp",
}};

std::array<int, 18> OpcodeParams
{{
    1, //pushNum,
    1, //pushStr,
    1, //getVar,
    1, //setVar
    0, //pop,
    0, //add,
    0, //sub,
    0, //mult,
    0, //_div,
    0, //mod,
    0, //_and,
    0, //_or,
    0, //eq,
    1, //call,
    0, //_return
    1, // jf
    1, // jt
    1, // jmp
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
    ss << "==== Strings" << std::endl;
    for (auto s : strTable)
    {
        ss << s << std::endl;
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
        ss << OpcodeStrings[(size_t)iv[i]];
        for (int p = 1; p <= OpcodeParams[iv[i]]; ++p)
        {
            if (iv[i] == opcode::pushNum)
            {
                uint64_t num = iv[i+p];
                ss << " " << *reinterpret_cast<double*>(&num);
            }
            else
            {
                ss << " " << (int64_t)iv[i+p];
            }
        }
        i += OpcodeParams[iv[i]];
        ss << std::endl;
    }

    return ss.str();
}

std::vector<uint64_t> Compiler::Compile()
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
            uint64_t funcAddr = iv.size();

            varStack.emplace_back();
            for (int i = 1; i < n.c.size() - 1; ++i)
            {
                auto name = n.c[i].tok->text.to_string();
                params.emplace_back(n.c[i].tok->text);
                varStack.back().emplace_back(name, -i - 1);
                varOffsets[name] = -i - 1;
            }
            currVarOffset = 0;
            FindVarDecls(n);
            functions[name] = { funcAddr, params };
            Compile(n.c[n.c.size() - 1]);
            iv.push_back(opcode::_return);
            varStack.pop_back();
            varOffsets.clear();
            currVarOffset = 0;
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
        if (n.tok && n.tok->type == TokenType::Let)
        {
            auto name = n.c[0].tok->text.to_string();
            varStack.back().emplace_back(name, varOffsets[name]);
            Compile(n.c[1]);
            iv.insert(iv.end(), {opcode::setVar, (uint64_t)varOffsets[name]});
        }
        else if (n.tok && n.tok->type == TokenType::If)
        {
            Compile(n.c[0]);
            iv.push_back(opcode::jf);
            iv.push_back(0);
            auto jParamIndex = iv.size() - 1;
            Compile(n.c[1]);
            auto jParam = iv.size();
            if (n.c.size() >= 3)
            {
                iv.push_back(opcode::jmp);
                iv.push_back(0);
                jParam = iv.size();
                auto elseParamIndex = iv.size() - 1;
                Compile(n.c[2]);
                iv[elseParamIndex] = iv.size();
            }
            iv[jParamIndex] = jParam;
        }
        else
        {
            Compile(n.c);
            if (n.tok && n.tok->type == TokenType::Return)
            {
                iv.push_back(opcode::_return);
            }
            else
            {
                iv.push_back(opcode::pop);
            }
        }
    }
    break;

    case OpAssign:
    {
        auto v = FindVar(n.c[0].tok->text.to_string());
        auto copyC = std::vector<Node>(n.c.begin() + 1, n.c.end());
        Compile(copyC);
        iv.insert(iv.end(), {opcode::setVar, (uint64_t)varOffsets[v.name]});
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
            iv.push_back(opcode::_and);
            break;
        case TokenType::Or:
            iv.push_back(opcode::_or);
            break;
        case TokenType::Equal:
            iv.push_back(opcode::eq);
            break;
        case TokenType::Add:
            iv.push_back(opcode::add);
            break;
        case TokenType::Sub:
            iv.push_back(opcode::sub);
            break;
        case TokenType::Mult:
            iv.push_back(opcode::mult);
            break;
        case TokenType::Div:
            iv.push_back(opcode::_div);
            break;
        case TokenType::Mod:
            iv.push_back(opcode::mod);
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
        iv.insert(iv.end(), {opcode::call, (uint64_t)std::distance(functions.begin(), fi)});
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
            double num = atof(std::string(n.tok->text).c_str());
            iv.insert(iv.end(), {opcode::pushNum, *reinterpret_cast<uint64_t*>(&num)});
        }
        break;
        case TokenType::Ident:
        {
            auto v = FindVar(n.tok->text.to_string());
            iv.insert(iv.end(), {opcode::getVar, (uint64_t)v.offset});
        }
        break;
        case TokenType::String:
        {
            strTable.emplace_back(n.tok->text);
            iv.insert(iv.end(), {opcode::pushStr, strTable.size() - 1});
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

std::tuple<std::map<std::string,Function>,std::vector<std::string>,std::vector<uint64_t>> Compiler::Serialize()
{
    /*auto bytecode = std::vector<uint64_t>();
    for (auto i : iv)
    {
        bytecode.push_back((uint64_t)i.op);
        for (auto p : i.p)
        {
            bytecode.push_back((uint64_t)p);
        }
    }*/
    return std::make_tuple(functions, strTable, iv);
}

void Compiler::FindVarDecls(const Node& n)
{
    if (n.tok && n.tok->type == TokenType::Let)
    {
        iv.push_back(opcode::pushNum);
        iv.push_back(0);
        varOffsets[n.tok->text.to_string()] = currVarOffset;
        currVarOffset++;
    }
    for (auto c : n.c)
    {
        FindVarDecls(c);
    }
}
