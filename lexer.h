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

struct TTDetail
{
  TokenType type;
  std::string str;
};

std::array<TTDetail, 4> TokenDetailsVarChar =
{{
  {TokenType::Let, "let"},
  {TokenType::For, "for"},
  {TokenType::While, "while"},
  {TokenType::Return, "return"},
}};

std::array<TTDetail, 19> TokenDetailsOneChar =
{{
  {TokenType::Colon, ":"},
  {TokenType::LCurly, "{"},
  {TokenType::RCurly, "}"},
  {TokenType::LParen, "("},
  {TokenType::RParen, ")"},
  {TokenType::Semicolon, ";"},
  {TokenType::LSquare, "["},
  {TokenType::RSquare, "]"},

  {TokenType::Assign, "="},
  //{TokenType::Negate, "-"}, // TODO special?
  {TokenType::Not, "!"},
  {TokenType::Mult, "*"},
  {TokenType::Div, "/"},
  {TokenType::Mod, "%"},
  {TokenType::Add, "+"},
  {TokenType::Sub, "-"},
  {TokenType::Less, "<"},
  {TokenType::Greater, ">"},
  {TokenType::Comma, ","},
  {TokenType::Call, "."},
}};

std::array<TTDetail, 8> TokenDetailsTwoChar =
{{
  {TokenType::Increment, "++"},
  {TokenType::Decrement, "--"},
  {TokenType::Equal, "=="},
  {TokenType::NotEqual, "!="},
  {TokenType::LessEq, "<="},
  {TokenType::GreaterEq, ">="},
  {TokenType::And, "&&"},
  {TokenType::Or, "||"},
}};

std::array<TTDetail, 4> TokenDetailsOther =
{{
  {TokenType::BadToken, "BadToken"},
  {TokenType::Ident, "Ident"},
  {TokenType::String, "String"},
  {TokenType::Number, "Number"},
}};

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
