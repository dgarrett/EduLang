#pragma once

#include <experimental/string_view>
#include <vector>
#include <array>
#include <map>
#include <experimental/optional>

using namespace std::experimental;

enum class TokenType
{
  BadToken,
  // Keywords
  Let,
  For,
  While,
  Return,
  If,
  Else,
  True,
  False,
  Underscore,

  // Symbols
  Colon,
  LCurly,
  RCurly,
  LParen,
  RParen,
  Semicolon,
  LSquare,
  RSquare,

  // Operators
  // 1 char
  Assign,
  //Negate,
  Not,
  Mult,
  Div,
  Mod,
  Add,
  Sub,
  Less,
  Greater,
  Comma,
  Call,
  // 2 char
  Increment,
  Decrement,
  Equal,
  NotEqual,
  LessEq,
  GreaterEq,
  And,
  Or,

  // Literals
  Ident,
  Number,
  String,
};

std::string& TokenName(TokenType type);

struct TTDetail
{
  TokenType type;
  std::string str;
};

struct Token
{
  std::experimental::string_view text;
  TokenType type;
};

class Lexer
{
public:
  Lexer(std::string fileContents);
  Lexer() = delete;

  std::vector<Token> Process();

  Token Current();
  optional<Token> Advance();

private:
  std::experimental::optional<Token> TrySymbolOrOperator();
  optional<Token> TryLiteral();
  optional<Token> TryIdentOrKeyword();
  bool ConsumeWhitespace();

  std::string m_fileContents;
  std::experimental::string_view v;
  int index;
  Token current;

};
