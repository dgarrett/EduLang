#include <iostream>
#include <sstream>
#include "parser.h"

std::array<std::string, 14> NodeTypeStrings
{{
    "Program",
    "Decl",
    "Ident",
    "Expr",
    "Block",
    "Statement",

    "OpAssign",
    "OpBool",
    "OpEq",
    "OpAdd",
    "OpMult",
    "OpUnary",
    "OpCallLookup",
    "OpEnd"
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
        ss << "|";
    }
    ss << "NodeType: " << NodeTypeStrings[n.t];
    if (n.tok)
    {
        ss << " Token: \"" << n.tok->text << "\" (" << TokenName(n.tok->type) << ")";
    }
    ss << std::endl;
    for (auto child : n.c)
    {
        ss << ToString(indent + 1, child);
    }

    return ss.str();
}

Token Parser::Expect(TokenType tt)
{
    std::vector<TokenType> tv;
    tv.push_back(tt);
    return Expect(tv);
}

Token Parser::Expect(std::vector<TokenType> tt)
{
    std::cout << "Expect: ";
    for (auto i : tt)
    {
        std::cout << TokenName(i) << " ";
    }
    std::cout << std::endl;
    auto res = Accept(tt);
    if (!res)
    {
        std::cout << "Failed expect, current token: " << TokenName(tokens.front().type) << std::endl;
        throw std::exception();
    }
    return *res;
}

optional <Token> Parser::Accept(TokenType tt)
{
    std::vector<TokenType> tv;
    tv.push_back(tt);
    return Accept(tv);
}

optional<Token> Parser::Accept(std::vector<TokenType> tt)
{
    std::cout << "Accept: ";
    for (auto i : tt)
    {
        std::cout << TokenName(i) << " ";
    }
    std::cout << std::endl;

    auto t = tokens.front();
    if (tt.end() != std::find(tt.begin(), tt.end(), t.type))
    {
        auto f = tokens.front();
        tokens.pop_front();
        return f;
    }
    else
    {
        return nullopt;
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

Node Parser::Decl()
{
    Node n;
    n.t = NodeType::Decl;
    Expect(TokenType::Let);
    n.c.push_back(Ident());
    if ((n.tok = Accept(TokenType::Equal)))
    {
        n.c.push_back(Expr());
    }
    else if ((n.tok = Accept(TokenType::Colon)))
    {
        optional<Token> t;
        while((t = Accept(TokenType::Ident)))
        {
            Node ident;
            ident.t = NodeType::Ident;
            ident.tok = t;
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
    n.t = NodeType::Ident;
    n.tok = Expect(TokenType::Ident);

    return n;
}

Node Parser::Block()
{
    Node n;
    n.t = NodeType::Block;
    Expect(TokenType::LCurly);
    optional<Node> s;
    while (!Accept(TokenType::RCurly))
    {
        n.c.push_back(Statement());
    }
    return n;
}

Node Parser::Statement()
{
    Node n;
    n.t = NodeType::Statement;
    if ((n.tok = Accept(TokenType::For)))
    {
        
    }
    else if ((n.tok = Accept(TokenType::While)))
    {
        
    }
    else if ((n.tok = Accept(TokenType::Return)))
    {
        
    }
    n.c.push_back(Expr());
    Expect(TokenType::Semicolon);
    return n;
}

Node Parser::Expr()
{
    return OpAssign();
}

Node Parser::OpAssign()
{
    Node n;
    n.t = NodeType::OpAssign;
    auto c = OpBool();
    if ((n.tok = Accept(TokenType::Assign)))
    {
        n.c.push_back(c);
        n.c.push_back(Expr());
        return n;
    }
    return c;
}

Node Parser::OpBool()
{
    Node n;
    n.t = NodeType::OpBool;
    auto c = OpEq();
    if ((n.tok = Accept({TokenType::And, TokenType::Or})))
    {
        n.c.push_back(c);
        n.c.push_back(OpBool());
        return n;
    }
    return c;
}

Node Parser::OpEq()
{
    Node n;
    n.t = NodeType::OpEq;
    auto c = OpAdd();
    if ((n.tok = Accept({TokenType::Equal, TokenType::NotEqual, TokenType::Less, TokenType::Greater, TokenType::LessEq, TokenType::GreaterEq})))
    {
        n.c.push_back(c);
        n.c.push_back(OpEq());
        return n;
    }

    return c;
}

Node Parser::OpAdd()
{
    Node n;
    n.t = NodeType::OpAdd;
    auto c = OpMult();
    if ((n.tok = Accept({TokenType::Add, TokenType::Sub})))
    {
        n.c.push_back(c);
        n.c.push_back(OpAdd());
        return n;
    }
    return c;
}

Node Parser::OpMult()
{
    Node n;
    n.t = NodeType::OpMult;
    auto c = OpUnary();
    if ((n.tok = Accept({TokenType::Mult, TokenType::Div, TokenType::Mod})))
    {
        n.c.push_back(c);
        n.c.push_back(OpMult());
        return n;
    }
    return c;
}

Node Parser::OpUnary()
{
    Node n;
    n.t = NodeType::OpUnary;
    auto prefix = Accept({TokenType::Sub, TokenType::Increment, TokenType::Decrement, TokenType::Not});
    auto c = OpCallLookup();
    auto postfix = prefix ? nullopt : Accept({TokenType::Increment, TokenType::Decrement});
    if (prefix || postfix)
    {
        n.c.push_back(c);
        return n;
    }
    return c;
}

Node Parser::OpCallLookup()
{
    Node n;
    n.t = NodeType::OpCallLookup;
    auto c = OpEnd();
    if ((n.tok = Accept({TokenType::LParen, TokenType::LSquare})))
    {
        n.c.push_back(c);
        if ((*n.tok).type == TokenType::LParen)
        {
            if (!Accept(TokenType::RParen))
            {
                n.c.push_back(Expr());
                while (Accept(TokenType::Comma))
                {
                    n.c.push_back(Expr());
                }
                Expect(TokenType::RParen);
            }
            return n;
        }
        else if ((*n.tok).type == TokenType::LSquare)
        {
            n.c.push_back(Expr());
            Expect(TokenType::RSquare);
        }
        return n;
    }
    return c;
}

Node Parser::OpEnd()
{
    Node n;
    n.t = NodeType::OpEnd;
    if ((n.tok = Accept(TokenType::LParen)))
    {
        n.c.push_back(Expr());
        Expect(TokenType::RParen);
        return n;
    }
    n.tok = Expect({TokenType::Ident, TokenType::Number, TokenType::String});
    return n;
}
