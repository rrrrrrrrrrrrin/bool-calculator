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

void BooleanExpression::build_binary_tree(std::string formula, std::stack<char>& operators, std::stack<std::string>& operands, int& N)
{
	// Parse the string and build a binary tree
	size_t i = 0;
	while (i < formula.length())
	{
		char s = formula[i];

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
				char s1 = formula[++i];
				op1.push_back(s1);

				int N_temp = s1 - '0';
				N = N_temp > N ? N_temp : N;
			}

			operands.push(op1);
		}

		++i;
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

static void to_binary(int i, std::vector<int>& buffer)
{
	while (i > 0)
	{
		buffer.insert(buffer.begin(), i % 2);
		i = i / 2;
	}
}

std::string BooleanExpression::table()
{
	int N = 0;  // variables amount

	std::stack<char> operators;
	std::stack<std::string> operands;

	build_binary_tree(formula_, operators, operands, N);

	std::string res;

	// Bitmasking to iterate through numbers from 0 to 2^N (1 << N),
	// where 2^N is the amount of possible functions for N variables
	for (int i = 0; i < (1 << N); i++)
	{
		std::vector<int> buffer;
		to_binary(i, buffer);

		if (buffer.size() < N)
		{
			while (buffer.size() != N)
			{
				buffer.push_back(0);
			}
		}

		/*
			Unpack each operand, insert bool values instead
			Change all operators: union ('v') - ||, intersection ('&') - &&
			Construct a logical expression to get a bool result
		*/
		
		std::stack<bool> func_bool;

		// Construct a formula
		while (!operators.empty())
		{
			char op = operators.top();

			if (op == ops[0])
			{
				push_negation(operators, operands);
			}

			std::stack<char> operators_new;
			std::stack<std::string> operands_new;

			int temp = 0;
			build_binary_tree(operands.top(), operators_new, operands_new, temp);
			operands.pop();

			if (operators_new.empty())
			{
				std::string x = operands_new.top();
				operands_new.pop();

				int val_x = x[x.size() - 1] - '0' - 1;

				bool bool_x = buffer[val_x];

				if (x[0] == '~') { bool_x = !bool_x; };

				func_bool.push(bool_x);
			}

			while (!operators_new.empty())
			{
				char op = operators_new.top();
				operators_new.pop();

				if (op == ops[0])
				{
					push_negation(operators_new, operands_new);
				}

				std::string x = operands_new.top();
				operands_new.pop();

				std::string y = operands_new.top();
				operands_new.pop();

				// Variable's number (-1) to apply the appropriate bool value from buffer
				int val_x = x[x.size() - 1] - '0' - 1;
				int val_y = y[y.size() - 1] - '0' - 1;

				bool bool_x = buffer[val_x];
				bool bool_y = buffer[val_y];

				if (x[0] == '~') { bool_x = !bool_x; };

				if (y[0] == '~') { bool_y = !bool_y; };

				if (op == ops[1])  // '&'
				{
					func_bool.push(bool_x && bool_y);
				}
				else  // 'v'
				{
					func_bool.push(bool_x || bool_y);
				}
			}

			if (func_bool.size() > 1)
			{
				bool x = func_bool.top();
				func_bool.pop();

				bool y = func_bool.top();
				func_bool.pop();

				if (op == ops[1])  // '&'
				{
					func_bool.push(x && y);
				}
				else  // 'v'
				{
					func_bool.push(x || y);
				}

				operators.pop();
			}
		}

		res.push_back(func_bool.top() ? '1' : '0');
	}

	return BooleanExpression(res.c_str());
}

bool BooleanExpression::isFullSystem(const std::vector<BooleanExpression>&)
{
	return false;
}