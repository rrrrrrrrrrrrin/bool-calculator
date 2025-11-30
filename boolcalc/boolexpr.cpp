#include "boolexpr.h"

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

void BooleanExpression::push_negation(std::stack<char>& operators, std::stack<std::string>& operands, char op)
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
	else if (x[0] == op)
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

	// TODO: Distribute x and y (x v y) & (x v y) when receiving operands (only with & and v)

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
		operands.push('(' + de_morgan_law(x) + " & " + de_morgan_law(y) + ')' + " v " + '(' + x + " & " + y + ')');
	}

	operators.pop();
}

void BooleanExpression::new_operand(std::stack<char>& operators, std::stack<std::string>& operands, size_t idx)
{
	/*
		Express logical operations in terms of '&' and 'v'
		Put operands in their stack, calculating new operands if needed, based on priority of operations:

		if the expression is in '()', it is calculated and put in operands;

		if a current operator has a lesser priority than a previous one, calculate a new operand, put in operands
	*/

	char op = operators.top();

	if (std::find(ops.begin(), ops.begin() + idx, op) != ops.end())
	{
		push_operand(operators, operands, op);
	}
}

void BooleanExpression::operand(std::stack<char>& operators, std::stack<std::string>& operands, char op)
{
	for (size_t i = 1; i < ops.size(); i++)
	{
		if (op == ops[i])
		{
			new_operand(operators, operands, i);
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

		if (s == '~')   // '~' has the highest priority =>
		{
			if (!operators.empty())
			{
				push_negation(operators, operands, s);  // => push negation immediately into operands
			}
			else
			{
				operators.push(s);
			}
		}

		// if the symbol is an operator
		else if (std::find(ops.begin() + 1, ops.end(), s) != ops.end())
		{
			char op = operators.top();
			if (op == ops[0])  // '~'
			{
				push_negation(operators, operands, s);
			}
			else if (operands.size() > 1)
			{
				if (op != '(')
				{
					operand(operators, operands, op);
				}
			}

			operators.push(s);  // push new operator
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
			op1.push_back(s);

			if (s != '0' && s != '1')
			{
				char s1 = formula_[++i];
				op1.push_back(s1);
			}

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