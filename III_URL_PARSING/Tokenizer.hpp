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
		case '@' :
			return { SymbolType::at,  std::string{Char} };
		case '.':
			return { SymbolType::dot,  std::string{Char} };
		case '\\':
		{

		}


		default:

			if (Char >= '0' && Char <= '9')
			{
				Token digits{ SymbolType::digits,std::string{Char} };
				
				while (true)
				{
					char nextChar = m_scanner.PeekNextChar();
					if (nextChar >= '0' && nextChar <= '9')
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

			if (Char > 'a' && Char < 'z' || Char>'A' && Char < 'Z')
			{
				Token text{ SymbolType::text, std::string{Char} };

				while()

			}



			return { SymbolType::unknown,  std::string{Char} }; //SSO avoids heap alloc
		}
	}

private:
	Scanner m_scanner;

};

