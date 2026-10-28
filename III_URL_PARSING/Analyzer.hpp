#pragma once
#include<optional>
#include<array>
#include<expected>
#include "Tokenizer.hpp"
class Analyzer
{
public:
	std::optional<Token> accept(SymbolType symbolType)
	{
		if(m_currentToken.symbolType == symbolType)
		{
			Token token = m_currentToken;
			m_currentToken = m_tokenizer.GetNextToken();
			return token;
		}

		return std::nullopt;

	}

	std::expected<Token,std::string> expect(SymbolType symbolType)
	{
		if(auto token = accept(symbolType))
		{ 
			return *token;
		}
		//error handling
		return std::unexpected{ "Неочакван Token" };
	}

private:

	auto Xalpha()
	{
		static constexpr std::array<SymbolType,5> validSymbols = 
		{ SymbolType::text, SymbolType::digits, SymbolType::escape, SymbolType::safe, SymbolType::extra };
	}

	
	


	Tokenizer m_tokenizer;
	Token m_currentToken;
};

