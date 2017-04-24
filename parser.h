#pragma once

#include <list>
#include <deque>
#include "lexer.h"

typedef std::list<Token>::iterator Ti;

enum NodeType
{
    Program,
    Decl,
    Assign,
    Func,
    Ident,
    Block,
};

struct Node
{
    NodeType t; // type
    std::vector<Node> c; // children
    optional<Token> tok;
};

class Parser
{
public:
    Parser(std::vector<Token> t) : tokens(t.begin(), t.end()) {}
    Node Parse();
    std::string ToString();

private:
    Token Expect(TokenType tt);
    optional<Token> Accept(TokenType tt);
    //void Check();
    std::string ToString(int indent, Node n);

    Node Program();
    Node Decl(Ti i);
    Node Ident(Ti i);
    Node Expr(Ti i);
    Node Block(Ti i);
    Node Statement(Ti i);

    std::list<Token> tokens;
    Ti end;
    Node p;
};
