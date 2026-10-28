#pragma once
#include<iostream>

class Scanner
{
	std::istream& m_stream;

public:
	Scanner(std::istream& stream = std::cin)
	:	m_stream(stream)
	{

	}



	char GetNextChar()
	{
		char Char;

		if (!m_stream.get(Char)) {
			Char = '\0';
		}

		return Char;
	}

	char PeekNextChar()
	{
		return m_stream.peek();
	}

};

