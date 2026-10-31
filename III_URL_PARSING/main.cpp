


#include "Analyzer.hpp"

int main()
{
	while (true)
	{

		
		if (auto result = Analyzer::Analyze(); !result)
		{
			std::cerr << "Error: " << result.error() << std::endl;

			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		
		}
		else
		{
			if (*result == Analysis::ValidUrl)
			{

				std::cout << "URL is valid." << std::endl;
			}
			//if url is empty, we don't print
		}
	}
	return 0;
}
