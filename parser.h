#pragma once

#include <list>
#include <deque>
#include "lexer.h"

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
    Node Decl();
    Node Ident();
    Node Expr();
    Node Block();
    Node Statement();

    std::deque<Token> tokens;
    std::list<Token>::iterator end;
    Node p;
};
