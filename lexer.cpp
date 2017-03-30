#include "lexer.h"
#include <iostream>
#include <sstream>
#include <fstream>

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
  //std::cout << "Result: " << result << std::endl;
  return 0;
}
