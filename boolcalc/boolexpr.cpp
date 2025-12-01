#include "boolexpr.h"
#include <cstdlib>
#include <iostream>

std::string BooleanExpression::de_morgan_law(std::string x)
{
	std::string res;
	for (size_t i = 0; i < x.length(); i++)
	{
		char s = x[i];

		if (s == '~')  // Apply the involutive law
		{
			res.push_back(x[++i]);  // skip an operand afterwards
		}
		else if (s == ops[1])
		{
			res.push_back(ops[2]);
		}
		else if (s == ops[2])
		{
			res.push_back(ops[1]);
		}
		else if (s == 'x')  // operand
		{
			res.push_back('~');
			res.push_back(s);
		}
		else
		{
			res.push_back(s);
		}
	}

	return res;
}

void BooleanExpression::push_negation(std::stack<char>& operators, std::stack<std::string>& operands)
{
	std::string x = operands.top();
	operands.pop();

	if (x == "0")
	{
		operands.push("1");
	}
	else if (x == "1")
	{
		operands.push("0");
	}
	// Apply the involutive law (~~x = x)
	else if (x[0] == '~')
	{
		operands.push(x);
	}
	else
	{
		operands.push(de_morgan_law(x));
	}

	operators.pop();
}

void BooleanExpression::push_operand(std::stack<char>& operators, std::stack<std::string>& operands, char op)
{
	std::string x = operands.top();
	operands.pop();

	std::string y = operands.top();
	operands.pop();

	if (op == ops[1])
	{
		operands.push(x + " & " + y);
	}
	else if (op == ops[2])  // 'v' 
	{
		operands.push(x + " v " + y);
	}
	else if (op == ops[3])  // '+'
	{
		operands.push(x + " & " + de_morgan_law(y) + " v " + de_morgan_law(x) + " & " + y);
	}
	else if (op == ops[4])  // '|'
	{
		operands.push(de_morgan_law(x) + " v " + de_morgan_law(y));
	}
	else if (op == ops[5])  // '^'
	{
		operands.push(de_morgan_law(x) + " & " + de_morgan_law(y));
	}
	else if (op == ops[6])  // '<'
	{
		operands.push(x + " v " + de_morgan_law(y));
	}
	else if (op == ops[7])  // '>'
	{
		operands.push(de_morgan_law(x) + " v " + y);
	}
	else if (op == ops[8])  // '='
	{
		// operands.push('(' + de_morgan_law(x) + " & " + de_morgan_law(y) + ')' + " v " + '(' + x + " & " + y + ')');
		operators.push('v');
		operands.push(de_morgan_law(x) + " & " + de_morgan_law(y));
		operands.push(x + " & " + y);
	}

	operators.pop();
}

void BooleanExpression::new_operand(std::stack<char>& operators, std::stack<std::string>& operands, char op_previous, char op_current)
{
	/*
		Put operands in their stack, calculating new operands if needed, based on priority of operations:
		if a current operator has a lesser priority than a previous one, calculate a new operand, put in operands
	*/

	// The lesser the index, the higher the priority
	if (std::find(ops.begin(), ops.end(), op_previous) < std::find(ops.begin(), ops.end(), op_current))
	{
		push_operand(operators, operands, op_previous);
	}
}

BooleanExpression BooleanExpression::cnf()
{
	return BooleanExpression("pass");
}

BooleanExpression BooleanExpression::dnf()
{
	std::string dnf;
	BooleanExpression result_dnf(dnf.c_str());
	return result_dnf;
}

BooleanExpression BooleanExpression::zhegalkin()
{
	return BooleanExpression("pass");
}

std::string BooleanExpression::table()
{
	int N = 2;  // variables amount

	std::stack<char> operators;
	std::stack<std::string> operands;

	// Parse the string and build a binary tree
	size_t i = 0;
	while (i < formula_.length())
	{
		char s = formula_[i];

		if (s == ' ')
		{
			++i;
			continue;
		}

		// if the symbol is an operator
		else if (std::find(ops.begin(), ops.end(), s) != ops.end())
		{
			if (operators.empty())
			{
				operators.push(s);
				++i;
				continue;
			}

			char op = operators.top();
			if (op == ops[0])
			{
				push_negation(operators, operands);
			}
			else
			{
				new_operand(operators, operands, op, s);
			}

			operators.push(s);  // push the current operator

		}
		// If the expression is in '()', it is calculated and put in operands
		else if (s == '(')
		{
			operators.push(s);
		}
		else if (s == ')')
		{
			char op = operators.top();
			while ((op = operators.top()) != '(')
			{
				if (op == ops[0])
				{
					push_negation(operators, operands);
				}
				else
				{
					push_operand(operators, operands, op);
				}
			}
			operators.pop();  // pop '('
		}
		// if the symbol is an operand
		else
		{
			std::string op1;
			op1.push_back(s);

			if (s != '0' && s != '1')
			{
				char s1 = formula_[++i];
				op1.push_back(s1);
				 
				N = s1 > N ? s1 : N;
			}

			operands.push(op1);
		}

		++i;
	}

	// Reconstruct a formula
	while (!operators.empty())
	{
		char op = operators.top();
		if (op == ops[0])
		{
			push_negation(operators, operands);
		}
		else
		{
			push_operand(operators, operands, op);
		}
	}

	std::string res;
	char buffer[33];  // N max is 5

	// Bitmasking to iterate through numbers from 0 to 2^n,
	// where 2^N is the amount of possible functions for N variables
	for (int i = 0; i < (1 << N); i++)
	{
		std::cout << _itoa_s(i, buffer, 2) << '\n';
	}

	return BooleanExpression(res.c_str());
}

bool BooleanExpression::isFullSystem(const std::vector<BooleanExpression>&)
{
	return false;
}