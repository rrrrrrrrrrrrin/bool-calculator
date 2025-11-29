#include "boolexpr.h"

void BooleanExpression::push_operand(std::stack<char>& operators, std::stack<std::string>& operands, char op)
{
	std::string x = operands.top();
	operands.pop();

	std::string y = operands.top();
	operands.pop();

	if (op == ops[0])  // '~'
	{
		operands.push(op + x);
		return;
	}

	// variables are written based on their indexes ascendingly
	if (x[1] > y[1])
	{
		std::swap(x, y);
	}

	if (op == ops[1])  // '&'
	{
		operands.push(x + " & " + y);
	} 
	else if (op == ops[2])  // 'v' 
	{
		operands.push(x + " v " + y);
	}
	else if (op == ops[3])  // '+'
	{
		operands.push(x + " & " + '~' + y + " v " + '~' + x + " & " + y);
	}
	else if (op == ops[4])  // '|'
	{
		operands.push('~' + x + " v " + '~' + y);
	}
	else if (op == ops[5])  // '^'
	{
		operands.push('~' + x + " & " + '~' + y);
	}
	else if (op == ops[6])  // '<'
	{
		operands.push(x + " v " + '~' + y);
	}
	else if (op == ops[7])  // '>'
	{
		operands.push('~' + x + " v " + y);
	}
	else if (op == ops[8])  // '='
	{
		operands.push("(~" + x + " & " + '~' + y + ')' + " v " + '(' + x + " & " + y + ')');
	}

	operators.pop();
}

void BooleanExpression::new_operand(std::stack<char> &operators, std::stack<std::string> &operands, int idx)
{
	/*
		Express logical operations in terms of '&' and 'v'
		Put operands in their stack, calculating new operands if needed, based on priority of operations:

		if the expression is in '()', it is calculated and put in operands;

		if a current operator has a lesser priority than a previous one, calculate a new operand, put in operands

		Apply the involution law (~~x = x)
	*/

	char op = operators.top();

	if (std::find(ops.begin(), ops.begin() + idx + 1, op) != ops.end())
	{
		push_operand(operators, operands, op);
	}
}

void BooleanExpression::operand(std::stack<char>& operators, std::stack<std::string>& operands, char op)
{
	for (size_t i = 0; i < ops.size(); i++)
	{
		if (op == ops[i])
		{
			new_operand(operators, operands, i);

			operators.push(op);
			break;
		}
	}
}

BooleanExpression BooleanExpression::cnf()
{
	return BooleanExpression("pass");
}

BooleanExpression BooleanExpression::dnf()
{
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
		if (std::find(ops.begin(), ops.end(), s) != ops.end())
		{
			if (!operators.empty())
			{
				char op = operators.top();
				operand(operators, operands, op);
			}
			else
			{
				operators.push(s);
			}
		}
		else if (s == '(')
		{
			operators.push(s);
		}
		else if (s == ')')
		{
			char op = operators.top();
			while ((op = operators.top()) != '(')
			{
				push_operand(operators, operands, op);
			}
			operators.pop();  // pop '('
		}
		// if the symbol is an operand
		else 
		{
			std::string op1;

			char s1 = formula_[++i];

			op1.push_back(s);
			op1.push_back(s1);

			operands.push(op1);
		}

		++i;
	}

	// Reconstruct a formula
	while (!operators.empty())
	{
		char op = operators.top();
		push_operand(operators, operands, op);
	}

	std::string dnf = operands.top();

	BooleanExpression result_dnf(dnf.c_str());
	return result_dnf;
}

BooleanExpression BooleanExpression::zhegalkin()
{
	return BooleanExpression("pass");
}

std::string BooleanExpression::table() 
{
	return "pass";
}

bool BooleanExpression::isFullSystem(const std::vector<BooleanExpression>&)
{
	return false;
}