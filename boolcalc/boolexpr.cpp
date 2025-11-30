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

	// Express logical operations in terms of '&' and 'v'. Apply a distributive law (for '&' and 'v') (use the cartesian product, i.e. nested loops for two operands x and y and push it as a new operand)
	if (op == ops[1])  // '&'
	{
		if (x.length() > 3 || y.length() > 3)  // (~)x(y)N
		{
			std::stack<char> operators_x;
			std::vector<std::string> operands_x;

			for (size_t i = 0; i < x.length(); i++)
			{
				char s = x[i];

				if (s == ' ')
				{
					++i;
					continue;
				}

				// if the symbol is an operator
				if (std::find(ops.begin() + 1, ops.end(), s) != ops.end())
				{
					operators_x.push(s);

				}
				// if the symbol is an operand
				else
				{
					std::string op1;
					op1.push_back(s);

					if (s == '~')
					{
						char s1 = x[++i];
						op1.push_back(s1);
					}

					if (s != '0' && s != '1')
					{
						char s2 = x[++i];
						op1.push_back(s2);
					}

					operands_x.push_back(op1);
				}

				++i;
			}

			std::vector<char> operators_y;
			std::vector<std::string> operands_y;

			for (size_t i = 0; i < y.length(); i++)
			{
				char s = y[i];

				if (s == ' ')
				{
					++i;
					continue;
				}

				// if the symbol is an operator
				if (std::find(ops.begin() + 1, ops.end(), s) != ops.end())
				{
					operators_y.push_back(s);

				}
				// if the symbol is an operand
				else
				{
					std::string op1;
					op1.push_back(s);

					if (s == '~')
					{
						char s1 = y[++i];
						op1.push_back(s1);
					}

					if (s != '0' && s != '1')
					{
						char s2 = y[++i];
						op1.push_back(s2);
					}

					operands_y.push_back(op1);
				}

				++i;
			}

			std::string new_operand;
			for (size_t i = 0; i < operands_x.size() - 1; i++)
			{
				std::string xi = operands_x[i];

				for (size_t j = 0; j < operands_y.size() - 1; j++)
				{
					std::string yj = operands_y[j];

					// Drop xN & ~xN(= 0), collapse (~)xN & (~)xN = (~)xN
					if ( (xi[0] == '~' && yj[0] != '~') || (yj[0] == '~' && xi[0] != '~') )
					{
						new_operand.push_back(operators_y[operators_y.size() - j - 1]);
						new_operand.push_back(' ');
					}
					else if (xi == yj)
					{
						new_operand += xi + ' ' + operators_y[operators_y.size() - j - 1] + ' ';
					}
					else
					{
						new_operand += xi + " & " + yj + ' ' + operators_y[operators_y.size() - j - 1] + ' ';
					}
				}

				new_operand += xi + " & " + operands_y[operands_y.size() - 1] + ' ' + operators_x.top() + ' ';
				operators_x.pop();
			}

			new_operand += operands_x[operands_x.size() - 1] + " & " + operands_y[operands_y.size() - 1];

			operands.push(new_operand);
		}
		else
		{
			operands.push(x + " & " + y);
		}
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