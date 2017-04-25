#include "compiler.h"

void Compiler::Compile(std::vector<Node> nv)
{
    for (auto n : nv)
    {
        Compile(n);
    }
}

void Compiler::Compile(Node n)
{
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
            //
        }
    }
    case Ident:
    case Expr:
    case Block:
    case Statement:

    case OpAssign:
    case OpBool:
    case OpEq:
    case OpAdd:
    case OpMult:
    case OpUnary:
    case OpCallLookup:
    case OpEnd:
    default:
        throw std::exception();
        break;
    }
}
