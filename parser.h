#pragma once

#include <list>
#include <deque>
#include "lexer.h"

enum NodeType
{
    Program,
    Decl,
    Ident,
    Expr,
    Block,
    Statement,

    OpAssign,
    OpBool,
    OpEq,
    OpAdd,
    OpMult,
    OpUnary,
    OpCallLookup,
    OpEnd
};

extern std::array<std::string, 14> NodeTypeStrings;

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
    Token Expect(std::vector<TokenType> tt);
    optional<Token> Accept(TokenType tt);
    optional<Token> Accept(std::vector<TokenType> tt);
    //void Check();
    std::string ToString(int indent, Node n);

    Node Program();
    Node Decl();
    Node Ident();
    Node Expr();
    Node Block();
    Node Statement();

    Node OpAssign();
    Node OpBool();
    Node OpEq();
    Node OpAdd();
    Node OpMult();
    Node OpUnary();
    Node OpCallLookup();
    Node OpEnd();

    std::deque<Token> tokens;
    std::list<Token>::iterator end;
    Node p;
};
