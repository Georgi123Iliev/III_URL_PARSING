#pragma once

#include<string>


enum class SymbolType
{
	plus,
	dot,
	slash,
	doubleSlash,
	http,
	ftp,
	colon,
	at,
	safe,
	extra,
	escape,
	digits,
	text,
	unknown,
	eof
};

struct Token
{
	SymbolType symbolType;
	std::string value; //обхваща всички нужди - няма смисъл от съхраняване на като число на digits

	Token(SymbolType symbolType, const std::string& value)
		: symbolType(symbolType), value(value)
	{

	}
};




