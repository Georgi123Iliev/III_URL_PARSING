#pragma once
#include "Scanner.hpp"
#include "Token.hpp"
class Tokenizer
{
public:



	Token GetNextToken()
	{
		// nothing to skip

		char Char = m_scanner.GetNextChar();

		switch (Char)
		{


		case '$':
		case '-':
		case '_':
		case '&':
			return { SymbolType::safe, std::string{ Char } };


		case '!':
		case '*':
		case '"':
		case '\'':
		case '(':
		case ')':
		case ';':
		case ',':
			//: and . are used elsewhere as well
		case ' ':

			return { SymbolType::extra, std::string{ Char } };

		case '/':
		{
			//can be / or //

			char nextChar = m_scanner.PeekNextChar();



			if (nextChar == '/')
			{
				nextChar = m_scanner.GetNextChar();

				return { SymbolType::doubleSlash,  std::string{Char,nextChar} };
			}

			return { SymbolType::slash,  std::string{Char} };
		}

		case '+':
			return { SymbolType::plus, std::string{Char} };
		case ':':
			return { SymbolType::colon,  std::string{Char} };
		case '@':
			return { SymbolType::at,  std::string{Char} };
		case '.':
			return { SymbolType::dot,  std::string{Char} };
		case '\0':
			return { SymbolType::eof,  std::string{} };
		case '%':
		{
			Token escape{ SymbolType::escape, std::string{Char} };

			char nextChar = m_scanner.PeekNextChar();

			auto isHexDigit = [](char c) {
				return (c >= '0' && c <= '9') ||
					(c >= 'a' && c <= 'f') ||
					(c >= 'A' && c <= 'F');
				};
			if (!isHexDigit(nextChar))
			{
				return { SymbolType::unknown,  escape.value };
			}

			
			escape.value.push_back(m_scanner.GetNextChar());

			

			nextChar = m_scanner.PeekNextChar();

			if (!isHexDigit(nextChar))
			{
				return { SymbolType::unknown,  escape.value };
			}

			escape.value.push_back(m_scanner.GetNextChar());
			return escape;
			

		}

		default:

			auto isAlpha = [](char c) {
				return (c >= 'a' && c <= 'z') ||
					(c >= 'A' && c <= 'Z');
				};

			auto isDigit = [](char c) {
				return (c >= '0' && c <= '9');
				};

			if (isDigit(Char))
			{
				Token digits{ SymbolType::digits,std::string{Char} };

				while (true)
				{
					char nextChar = m_scanner.PeekNextChar();
					if (isDigit(nextChar))
					{
						digits.value.push_back(m_scanner.GetNextChar());
					}
					else
					{
						break;
					}
				}

				return digits;
			}

			if (isAlpha(Char))
			{
				Token text{ SymbolType::text, std::string{Char} };

				while (true)
				{
					char nextChar = m_scanner.PeekNextChar();
					if (isAlpha(nextChar))
					{
						text.value.push_back(m_scanner.GetNextChar());
					}
					else
					{
						break;
					}
				}

				return text;

			}



			return { SymbolType::unknown,  std::string{Char} };
		}
	}

	private:
		Scanner m_scanner;
	
};

