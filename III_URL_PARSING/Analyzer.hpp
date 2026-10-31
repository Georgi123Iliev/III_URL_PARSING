#pragma once
#include<optional>
#include<array>
#include<expected>
#include<span>
#include "Tokenizer.hpp"

enum class Analysis
{
	EmptyUrl,
	ValidUrl
};

class Analyzer
{

public:

	using AnalysisResult = std::expected<Analysis, std::string>;
	static AnalysisResult Analyze(std::istream& input = std::cin)
	{
		auto analyzer = Analyzer(input);

		return analyzer.Url();
	}

private:
	Analyzer(std::istream& input)
		: m_tokenizer(input), m_currentToken(m_tokenizer.GetNextToken())
	{

	}
	using ParseResult = std::expected<void, std::string>;
	



	[[nodiscard]] std::optional<Token> accept(SymbolType symbolType)
	{
		if (m_currentToken.symbolType == symbolType)
		{
			Token token = m_currentToken;
			m_currentToken = m_tokenizer.GetNextToken();
			m_tokenChanged = true;
			return token;
		}

		return std::nullopt;

	}

	[[nodiscard]] std::optional<Token> accept(std::span< const SymbolType> symbolType)
	{

		for(auto& type : symbolType)
		{
			if (auto token = accept(type))
			{
				return token;
			}
		}

		return std::nullopt;

	}



	std::expected<Token, std::string> expect(SymbolType symbolType)
	{
		if (auto token = accept(symbolType))
		{
			return *token;
		}
		//error handling
		return std::unexpected{ "Unexpected Token [" + m_currentToken.value +"] where " + " was expected"};
	}

	

	ParseResult Xalpha()
	{
		static constexpr std::array<SymbolType, 5> validSymbols =
		{ SymbolType::text, SymbolType::digits, SymbolType::escape, SymbolType::safe, SymbolType::extra };

		std::string Xalphas = "";

		if (auto xalphaOpt = accept(validSymbols))
		{
			return {};
		}
		else
		{
			return std::unexpected{ "Xalpha: Unexpected token: \'" + m_currentToken.value +"\'"};
		}

		

	}

	ParseResult Xalphas()
	{
		if (auto xalphaExp = Xalpha(); !xalphaExp)
		{
			return std::unexpected{ xalphaExp.error() };
		}

		while (Xalpha())
		{
			//continue parsing Xalpha
		}

		return {};
	}


	//could probably avoid repetition here

	ParseResult Xpalpha()
	{
		if(Xalpha() || accept(SymbolType::plus))
		{
			return {};
		}
		else
		{
			return std::unexpected{ "Xpalpha: Unexpected token: \'" + m_currentToken.value + "\'" };
		}
	}

	ParseResult Xpalphas()
	{
		if (auto xpalphaExp = Xpalpha(); !xpalphaExp)
		{
			return std::unexpected{ xpalphaExp.error() };
		}
		while (Xpalpha())
		{
			//continue parsing Xpalpha
		}
		return {};
	}

	//Should be Ialphas?
	ParseResult Ialpha()
	{
		if (!accept(SymbolType::text))
		{
			return std::unexpected{ "Ialpha: Unexpected token: \'" + m_currentToken.value + "\'" };
		}

		std::ignore = Xalphas();

		return {};
	}

	ParseResult Password()
	{
		if (auto xalphasExp = Xalphas(); !xalphasExp)
		{
			return std::unexpected{"Unexpected in password: " + xalphasExp.error()};
		}

		return {};
	}

	ParseResult User()
	{
		if (auto xalphasExp = Xalphas(); !xalphasExp)
		{
			return std::unexpected{ "Unexpected in user: " + xalphasExp.error() };
		}

		return {};
	}

	ParseResult Path()
	{
		if (auto xalphasExp = Xalphas(); !xalphasExp)
		{
			if (!accept(SymbolType::epsilon))
			{
				return {};
			}

			return std::unexpected{ "Unexpected in path: " + xalphasExp.error() };
		}

		//should maybe limit recursion depth here
		if(accept(SymbolType::slash))
		{
			if (auto pathExp = Path(); !pathExp)
			{
				return std::unexpected{ "Unexpected in path: " + pathExp.error() };
			}
		}



		return {};
	}

	ParseResult Port()
	{
		if (auto digitsExp = expect(SymbolType::digits); !digitsExp)
		{
			return std::unexpected{ "Unexpected in port: " + digitsExp.error() };
		}
		return {};
	}

	ParseResult Hostnumber()
	{
		for( int i = 0; i < 4; ++i)
		{
			if (auto digitsExp = expect(SymbolType::digits); !digitsExp)
			{
				return std::unexpected{ "Unexpected in hostnumber: " + digitsExp.error() };
			}
			if (i < 3)
			{
				if (auto dotExp = expect(SymbolType::dot); !dotExp)
				{
					return std::unexpected{ "Unexpected in hostnumber: " + dotExp.error()};
				}
			}
		}

		


		return {};
	}

	ParseResult Hostname()
	{
		if (auto ialphaExp = Ialpha(); !ialphaExp)
		{
			return std::unexpected{ "Unexpected in hostname: " + ialphaExp.error() };
		}
		
		if(accept(SymbolType::dot))
		{
			if (auto hostnameExp = Hostname(); !hostnameExp)
			{
				return std::unexpected{ "Unexpected in hostname: " + hostnameExp.error() };
			}
		}
		return {};
	}

	ParseResult Host()
	{
		if (auto hostnumberExp = Hostnumber(); hostnumberExp)
		{
			return {};
		}
		else if (auto hostnameExp = Hostname(); hostnameExp)
		{
			return {};
		}
		else
		{
			return std::unexpected{ "Unexpected in host: " + hostnumberExp.error() + " " + hostnameExp.error() };
		}
	}

	std::expected<void,std::string> Hostport()
	{
		if (auto hostExp = Host(); !hostExp)
		{
			return std::unexpected{ "Unexpected in hostport: " + hostExp.error() };
		}
		if (accept(SymbolType::colon))
		{
			if (auto portExp = Port(); !portExp)
			{
				return std::unexpected{ "Unexpected in hostport: " + portExp.error() };
			}
		}
		return {};
	}

	ParseResult Login()
	{
		//user is optional???
		if (auto userExp = User())
		{

			if (accept(SymbolType::colon))
			{
				if (auto passwordExp = Password(); !passwordExp)
				{
					return std::unexpected{ "Unexpected in login: " + passwordExp.error() };
				}
			}
			if (auto atExp = expect(SymbolType::at); !atExp)
			{
				return std::unexpected{ "Unexpected in login: " + atExp.error() };
			}
		}

		if(auto hostportExp = Hostport(); !hostportExp)
		{
			return std::unexpected{ "Unexpected in login: " + hostportExp.error() };
		}


		return {};
	}

	ParseResult FtpAddress()
	{
		if (auto ftpExp = expect(SymbolType::ftp); !ftpExp)
		{
			return std::unexpected{ "Unexpected in ftpaddress: " + ftpExp.error() };
		}
		if (auto colonExp = expect(SymbolType::colon); !colonExp)
		{
			return std::unexpected{ "Unexpected in ftpaddress: " + colonExp.error() };
		}
		if (auto doubleSlashExp = expect(SymbolType::doubleSlash); !doubleSlashExp)
		{
			return std::unexpected{ "Unexpected in ftpaddress: " + doubleSlashExp.error() };
		}
		if (auto loginExp = Login(); !loginExp)
		{
			return std::unexpected{ "Unexpected in ftpaddress: " + loginExp.error() };
		}
		if( auto pathExp = Path(); !pathExp)
		{
			return std::unexpected{ "Unexpected in ftpaddress: " + pathExp.error() };
		}
		
		return {};
		
	
	}

	ParseResult HttpAddress()
	{
		if (auto httpExp = expect(SymbolType::http); !httpExp)
		{
			return std::unexpected{ "Unexpected in httpaddress: " + httpExp.error() };
		}
		if (auto colonExp = expect(SymbolType::colon); !colonExp)
		{
			return std::unexpected{ "Unexpected in httpaddress: " + colonExp.error() };
		}
		if (auto doubleSlashExp = expect(SymbolType::doubleSlash); !doubleSlashExp)
		{
			return std::unexpected{ "Unexpected in httpaddress: " + doubleSlashExp.error() };
		}
		if(auto hostportExp = Hostport(); !hostportExp)
		{
			return std::unexpected{ "Unexpected in httpaddress: " + hostportExp.error() };
		}
		while(accept(SymbolType::slash))
		{
			if (auto pathExp = Path(); !pathExp)
			{
				return std::unexpected{ "Unexpected in httpaddress: " + pathExp.error() };
			}
		}

		return {};
		
	
	}

	ParseResult Scheme()
	{
		if( auto ialphaExp = Ialpha(); !ialphaExp)
		{
			return std::unexpected{ "Unexpected in scheme: " + ialphaExp.error() };
		}

		return {};
	}

	ParseResult Generic()
	{
		if (auto schemeExp = Scheme(); !schemeExp)
		{
			return std::unexpected{ "Unexpected in generic: " + schemeExp.error() };
		}
		if (auto colonExp = expect(SymbolType::colon); !colonExp)
		{
			return std::unexpected{ "Unexpected in generic: " + colonExp.error() };
		}
		if(auto pathExp = Path(); !pathExp)
		{
			return std::unexpected{ "Unexpected in generic: " + pathExp.error() };
		}
		
		return {};
	}

	AnalysisResult Url()
	{
		
		m_tokenChanged = false;

		auto httpAddressExp = HttpAddress();
		if ( httpAddressExp)
		{
			return {Analysis::ValidUrl};
		}

		if (m_tokenChanged)
		{
			return std::unexpected{ "Unexpected token in url: " + httpAddressExp.error()};
		}

		auto ftpAddressExp = FtpAddress();
		if (ftpAddressExp)
		{
			return { Analysis::ValidUrl };
		}

		if (m_tokenChanged)
		{
			return std::unexpected{ "Unexpected token in url: " + ftpAddressExp.error() };
		}

		auto genericExp = Generic();
		if (genericExp)
		{
			return { Analysis::ValidUrl };
		}

		if(m_tokenChanged)
		{
			return std::unexpected{ "Unexpected token in url: " + genericExp.error() };
		}

		if(accept(SymbolType::epsilon))
		{
			return { Analysis::EmptyUrl };
		}

		


		return std::unexpected{ "Unexpected token in url: " + m_currentToken.value };
		
		
	
	}











	Tokenizer m_tokenizer;
	Token m_currentToken;
	bool m_tokenChanged = false;
};