#include "boolexpr.h"
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
	const char* action = argv[1];

	if (std::strcmp(action, "-h") == 0 || std::strcmp(action, "?") == 0)
	{
		std::cout << "Usage: boolcalc -action input_file output_file\n";
		std::cout << "Actions\t: -table, -cnf (technically pcnf), -dnf (technically pdnf), -zh, -isfull";
		return 0;
	}

	if (argc != 4)
	{
		std::cout << "Use boolcalc -h or boolcalc ? for help\n";
		return 1;
	}

	std::ifstream input(argv[2]);
	if (!input)
	{
		std::cout << "Couldn't open input file: " << argv[2] << '\n';
		return 2;
	}

	std::ofstream output(argv[3]);
	if (!output)
	{
		std::cout << "Couldn't open output file: " << argv[3] << '\n';
		return 3;
	}

	std::string line;

	if (std::strcmp(action,"-table") == 0)
	{
		while (getline(input, line))
		{
			BooleanExpression boolexpr(line.c_str());

			output << boolexpr.table() << '\n';
		}
	}
	else if (std::strcmp(action, "-cnf") == 0)
	{
		try
		{
			while (getline(input, line))
			{
				BooleanExpression boolexpr(line.c_str());

				BooleanExpression res_boolexpr = boolexpr.cnf();

				std::string result_function = res_boolexpr;  // via operator BooleanExpression::std::string()

				output << result_function << '\n';
			}
		}
		catch (const char* error)
		{
			output << error << '\n';
			return 0;
		}
	}
	else if (std::strcmp(action, "-dnf") == 0)
	{
		while (getline(input, line))
		{
			try
			{
				BooleanExpression boolexpr(line.c_str());

				BooleanExpression res_boolexpr = boolexpr.dnf();

				std::string result_function = res_boolexpr;  // via operator BooleanExpression::std::string()

				output << result_function << '\n';
			}
			catch (const char* error)
			{
				output << error << '\n';
				return 0;
			}
		}
	}
	else if (std::strcmp(action,"-zh") == 0)
	{
		while (getline(input, line))
		{
			try
			{
				BooleanExpression boolexpr(line.c_str());

				BooleanExpression res_boolexpr = boolexpr.zhegalkin();

				std::string result_function = res_boolexpr;  // via operator BooleanExpression::std::string()

				output << result_function << '\n';
			}
			catch (const char* error)
			{
				output << error << '\n';
				return 0;
			}
		}
	}
	else if (std::strcmp(action, "-isfull") == 0)
	{
			try
			{
				BooleanExpression boolexpr("");

				std::vector<BooleanExpression> system;

				while (getline(input, line))
				{
					BooleanExpression func(line.c_str());
					system.push_back(func);
				}

				if (boolexpr.isFullSystem(system))
				{
					output << "yes" << '\n';
				}
				else
				{
					output << "no" << '\n';
				}
			}
			catch (const char* error)
			{
				output << error << '\n';
				return 0;
			}
	}
	else
	{
		std::cout << "Unknown action\nUse boolcalc -h or boolcalc ? for help\n";
		return 4;
	}


	input.close();
	output.close();

	return 0;
}