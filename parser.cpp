#include <iostream>
#include <sstream>
#include "parser.h"

std::array<std::string, 6> NodeTypeStrings
{{
    "Program",
    "Decl",
    "Assign",
    "Func",
    "Ident",
    "Block",
}};

Node Parser::Parse()
{
    p = Program();
    return p;
}

std::string Parser::ToString()
{
    return ToString(0, p);
}

std::string Parser::ToString(int indent, Node n)
{
    std::stringstream ss;
    for (int i = 0; i < indent; ++i)
    {
        ss << " ";
    }
    ss << "NodeType: " << NodeTypeStrings[n.t];
    if (n.tok)
    {
        ss << " Token: " << TokenName((*n.tok).type);
    }
    ss << std::endl;
    for (auto child : n.c)
    {
        for (int i = 0; i < indent; ++i)
        {
            ss << " ";
        }
        ss << "Child" << std::endl;
        ss << ToString(indent + 1, child);
    }
    ss << std::endl;

    return ss.str();
}

std::pair<Token, Ti> Parser::Expect(TokenType tt, Ti i)
{
    std::cout << "Expect: " << TokenName(tt) << std::endl;
    return *Accept(tt, i);
}

std::pair<optional<Token>, Ti> Parser::Accept(TokenType tt, Ti i)
{
    std::cout << "Accept: " << TokenName(tt) << std::endl;
    if ((*i).type == tt)
    {
        return std::make_pair(*i, ++i);
    }
    else
    {
        return std::make_pair(nullopt, i);
    }
}


Node Parser::Program()
{
    Node n;
    n.t = NodeType::Program;
    while (!tokens.empty())
    {
        n.c.push_back(Decl());
    }
    return n;
}

Node Parser::Decl(Ti i)
{
    Node n;
    std::tie(std::ignore, i) = Expect(TokenType::Let);
    n.c.push_back(Ident());
    if (Accept(TokenType::Equal))
    {
        n.t = NodeType::Assign;
        n.c.push_back(Expr());
    }
    else if (Accept(TokenType::Colon))
    {
        n.t = NodeType::Func;
        optional<Token> t;
        while((t = Accept(TokenType::Ident)))
        {
            Node ident;
            ident.t = NodeType::Ident;
            ident.tok = *t;
            n.c.push_back(ident);
        }
        n.c.push_back(Block());
    }
    else
    {
        throw std::exception();
    }
    return n;
}

Node Parser::Ident()
{
    Node n;
    n.tok = Expect(TokenType::Ident);
    n.t = NodeType::Ident;

    return n;
}

Node Parser::Expr()
{
    Node n;
    throw std::exception();
    return n;
}

Node Parser::Block()
{
    Node n;
    n.t = NodeType::Block;
    Expect(TokenType::LCurly);
    optional<Node> s;
    try
    {
        while (true)
        {
            n.c.push_back(Statement());
        }
    }
    catch (std::exception& e)
    {
        
    }
    Expect(TokenType::RCurly);
    return n;
}

Node Parser::Statement()
{
    Node n;
    if (Accept(TokenType::For))
    {
        
    }
    else if (Accept(TokenType::While))
    {
        
    }
    else if (Accept(TokenType::Return))
    {
        
    }
    n.c.push_back(Expr());
    Expect(TokenType::Semicolon);
    return n;
}
