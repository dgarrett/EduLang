#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"
#include <iostream>
#include <sstream>
#include <fstream>

std::array<TTDetail, 6> TokenDetailsVarChar =
{{
  {TokenType::Let, "let"},
  {TokenType::For, "for"},
  {TokenType::While, "while"},
  {TokenType::Return, "return"},
  {TokenType::If, "if"},
  {TokenType::Else, "else"},
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

bool IsWhiteSpace(char c)
{
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool IsValidIdent(char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

bool IsDigit(char c)
{
  return (c >= '0' && c <= '9');
}

std::string& TokenName(TokenType type)
{
  static std::map<TokenType, std::string> tokenMap;
  if (tokenMap.empty())
  {
    for (TTDetail& det : TokenDetailsVarChar)
    {
      tokenMap[det.type] = det.str;
    }
    for (TTDetail& det : TokenDetailsOneChar)
    {
      tokenMap[det.type] = det.str;
    }
    for (TTDetail& det : TokenDetailsTwoChar)
    {
      tokenMap[det.type] = det.str;
    }
    for (TTDetail& det : TokenDetailsOther)
    {
      tokenMap[det.type] = det.str;
    }
  }
  return tokenMap[type];
}

Token Lexer::Current()
{
  return Token();
}

optional<Token> Lexer::Advance()
{
  if (!v.empty())
  {
    optional<Token> res;
    if ((res = TrySymbolOrOperator()) || (res = TryLiteral()) || (res = TryIdentOrKeyword()))
    {
      return res;
    }
    ConsumeWhitespace();
  }
  return nullopt;
}

bool Lexer::ConsumeWhitespace()
{
  bool res = false;
  while (IsWhiteSpace(v[0]))
  {
    v.remove_prefix(1);
    res = true;
  }
  return res;
}

optional<Token> Lexer::TrySymbolOrOperator()
{
  string_view substr;
  optional<TTDetail> res;

  // 2 char
  if (v.length() >= 2)
  {
    for (TTDetail& det : TokenDetailsTwoChar)
    {
      if (v[0] == det.str[0] && v[1] == det.str[1]) { res = det; break; }
    }
    if (res)
    {
      substr = v.substr(0,2);
      v.remove_prefix(2);
      return Token{substr, res->type};
    }
  }

  // 1 char
  for (TTDetail& det : TokenDetailsOneChar)
  {
    if (v[0] == det.str[0]) { res = det; break; }
  }
  if (res)
  {
    substr = v.substr(0,1);
    v.remove_prefix(1);
    return Token{substr, res->type};
  }

  return nullopt;
}

optional<Token> Lexer::TryIdentOrKeyword()
{
  int end = 0;
  while (end < v.size() && IsValidIdent(v[end]))
  {
    end++;
  }
  if (end > 0)
  {
    string_view substr = v.substr(0, end);
    v.remove_prefix(end);
    for (TTDetail& det : TokenDetailsVarChar)
    {
      if (substr == det.str)
      {
        return Token{substr, det.type};
      }
    }
    return Token{substr, TokenType::Ident}; // todo keywords
  }
  return nullopt;
}

optional<Token> Lexer::TryLiteral()
{
  // String
  if (v[0] == '"')
  {
    bool close = false;
    int i = 1;
    string_view substr;
    while (!close && i < v.length())
    {
      i++;
      if (v[i-1] == '"')
      {
        close = true;
        substr = v.substr(0, i);
        v.remove_prefix(i);
      }
    }
    return close ? optional<Token>(Token{substr, TokenType::String}) : nullopt;
  }

  // Number
  if (IsDigit(v[0]))
  {
    int i = 1;
    while (i < v.length() && (IsDigit(v[i]) || v[i] == '.'))
    {
      i++;
    }
    if (v[i] == '.')
    {
      // TODO ending in '.' be allowed?
      return nullopt;
    }
    string_view substr = v.substr(0, i);
    v.remove_prefix(i);
    return Token{substr, TokenType::Number};
  }

  return nullopt;
}

std::vector<Token> Lexer::Process()
{
  auto vec = std::vector<Token>();
  while (!v.empty())
  {
    optional<Token> res;
    std::cout << "size: " << v.size() << std::endl;
    if ((res = Advance()))
    {
      vec.push_back(*res);
      std::cout << "Found token: \"" << res->text << "\" (" << TokenName(res->type) << ")" << std::endl;
    }
    else
    {
      vec.push_back(Token{v, TokenType::BadToken});
      std::cout << "Error at: " << v << std::endl;
      return vec;
    }
    ConsumeWhitespace();
  }
  return vec;
}

Lexer::Lexer(std::string fileContents)
  : m_fileContents(fileContents)
  , index(0)
  , current()
  , v(m_fileContents)
{
  std::cout << "Tokenizing:" << std::endl << fileContents;
}

sv vmPrint(std::vector<sv>& stack)
{
    auto& param = stack[stack.size() - 2];
    std::cout << "****vmPrint: " << *param.heapVal->str << std::endl;
    return 0.f;
}

int main(int argc, char* argv[])
{
  std::ifstream file(argv[1]);
  std::stringstream buffer;
  buffer << file.rdbuf();
  Lexer l(buffer.str());
  auto tokens = l.Process();
  std::cout << "===Results===" << std::endl;
  for (auto& t : tokens)
  {
    std::cout << TokenName(t.type) << std::endl;
  }

  Parser p(tokens);
  Node program = p.Parse();
  std::cout << p.ToString();

  Compiler c(program);
  std::vector<uint64_t> instructions = c.Compile();
  std::cout << c.ToString();
  auto bytecode = c.Serialize();

  vm v(std::get<0>(bytecode), std::get<1>(bytecode), std::get<2>(bytecode));
  //v.Run("test", {{.type = svType::num, .num = 5}});
  v.Register("print", &vmPrint);
  sv res = v.Run("avg", {sv(5.f), sv(3.f), sv(2.f)});
  std::cout << "Result: " << res.num << std::endl;
  return 0;
}
