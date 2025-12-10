#include "boolexpr.h"
#include <algorithm>  // for std::sort
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
		operands.push(x + " + " + y);
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
		operands.push(x + " = " + y);
	}

	operators.pop();
}

void BooleanExpression::build_binary_tree(std::string formula, std::stack<char>& operators, std::stack<std::string>& operands, int& N, std::vector<char>& vals)
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

			char op_previous = operators.top();

			/*
				Put operands in their stack, calculating new operands if needed, based on priority of operations:
				if a current operator has a lesser priority than a previous one, calculate a new operand, put in operands
			*/

			char op_current = s;

			// The lesser the index, the higher the priority
			// op_previous is the previous operand, op_current is the current operand, s — we push to operators
			while (std::find(ops.begin(), ops.end(), op_previous) < std::find(ops.begin(), ops.end(), op_current))
			{
				if (op_previous == ops[0])
				{
					push_negation(operators, operands);

					// op_current stays the same, op_previous is the top of operators
				}
				else
				{
					push_operand(operators, operands, op_previous);

					op_current = op_previous;  // if op_current == op_previous, it won't push anything
				}

				if (!operators.empty()) 
				{ 
					op_previous = operators.top();
				}
				else
				{
					break;  // don't loop with the same op_previous and op_current
				}
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

				if (std::find(vals.begin(), vals.end(), s1) == vals.end())
				{
					vals.push_back(s1);
				}
			}

			operands.push(op1);
		}

		++i;
	}
}

BooleanExpression BooleanExpression::cnf()
{
	std::string cnf;

	/*
		Constructing a cnf: take disjunctions of variables (xi type) in the power of true or false respectfully 
		(if in the buffer for xi, 1 is saved => ~xi),
		when the function is FALSE in the truth table (for that need to know that iteration's buffer)
		Take a conjunction of all these disjunctions (... v ...) & (... v ...) (if there are several)
	*/

	std::string res = table();
	// => called void save_buffer(...) => saved a buffer in std::vector<std::vector<int>> buffers;
	// and occured variables in std::vector<char> vals

	// Sort vals_ by int values of contents in an descending order
	if (!vals_.empty())
	{
		auto sort_func = [](char val1, char val2) { return (val1 - '0') > (val2 - '0'); };
		std::sort(vals_.begin(), vals_.end(), sort_func);
	}
	else
	{
		cnf = res;
		BooleanExpression result_cnf(cnf.c_str());
		return result_cnf;
	}

	bool first = true;
	bool conjunction1 = false;
	bool conjunction2 = false;

	// The first buffer is at idx 0 in buffers; func_bool for the first buffer is at idx 0 in res (result of truth table)
	for (size_t i = 0; i < res.length(); i++)
	{
		if (res[i] == '0')
		{
			if (conjunction1)
			{
				if (first) {
					cnf.insert(cnf.begin(), '(');
					first = false;
				}

				cnf.append(") & (");

				conjunction2 = true;
			}

			std::vector<int> buffer = buffers[i];
			size_t size = buffer.size();

			// Variables are sorted by indexes in an ascending order (max i (x_i) bool value is at position 0 in buffer, etc.)

			/*
				buffer: idxs: 0 <- max variable, N-1 <- min variable
				vals_: idxs: 0 <- max var, N-1 <- min var
			*/

			for (size_t j = size - 1; j > 0; j--)
			{
				std::string xj;
				xj.push_back('x');

				xj.push_back(vals_[j]);

				if (buffer[j] == 1)
				{
					cnf.push_back('~');
				}

				cnf.append(xj);
				cnf.append(" v ");
			}

			if (buffer[0] == 1)
			{
				cnf.push_back('~');
			}
			cnf.push_back('x');
			cnf.push_back(vals_[0]);

			conjunction1 = true;
		}
	}

	if (conjunction2) { cnf.push_back(')'); }

	BooleanExpression result_cnf(cnf.c_str());
	return result_cnf;
}

BooleanExpression BooleanExpression::dnf()
{
	std::string dnf;

	/*
		Constructing a dnf: take conjunctions of variables (xi type) in the power of true or false respectfully,
		(if in the buffer for xi, 0 is saved => ~xi),
		when the function is TRUE in the truth table (for that need to know that iteration's buffer)
		Take a disjunction of all these conjunctions (... & ...) v (... & ...) (if there are several)
	*/

	std::string res = table();
	// => called void save_buffer(...) => saved a buffer in std::vector<std::vector<int>> buffers;
	// and occured variables in std::vector<char> vals

	// Sort vals_ by int values of contents in an descending order
	if (!vals_.empty())
	{
		auto sort_func = [](char val1, char val2) { return (val1 - '0') > (val2 - '0'); };
		std::sort(vals_.begin(), vals_.end(), sort_func);
	}
	else
	{
		dnf = res;
		BooleanExpression result_dnf(dnf.c_str());
		return result_dnf;
	}

	bool first = true;
	bool conjunction1 = false;
	bool conjunction2 = false;

	// The first buffer is at idx 0 in buffers; func_bool for the first buffer is at idx 0 in res (result of truth table)
	for (size_t i = 0; i < res.length(); i++)
	{
		if (res[i] == '1')
		{
			if (conjunction1)
			{
				if (first) {
					dnf.insert(dnf.begin(), '(');
					first = false;
				}

				dnf.append(") v (");

				conjunction2 = true;
			}

			std::vector<int> buffer = buffers[i];
			size_t size = buffer.size();

			// Variables are sorted by indexes in an ascending order (max i (x_i) bool value is at position 0 in buffer, etc.)

			/*
				buffer: idxs: 0 <- max variable, N-1 <- min variable
				vals_: idxs: 0 <- max var, N-1 <- min var
			*/

			for (size_t j = size - 1; j > 0; j--)
			{
				std::string xj;
				xj.push_back('x');

				xj.push_back(vals_[j]);

				if (buffer[j] == 0)
				{
					dnf.push_back('~');
				}

				dnf.append(xj);
				dnf.append(" & ");
			}

			if (buffer[0] == 0)
			{
				dnf.push_back('~');
			}
			dnf.push_back('x');
			dnf.push_back(vals_[0]);

			conjunction1 = true;
		}
	}

	if (conjunction2) { dnf.push_back(')'); }

	BooleanExpression result_dnf(dnf.c_str());
	return result_dnf;
}

BooleanExpression BooleanExpression::zhegalkin()
{
	return BooleanExpression("pass");
}

void BooleanExpression::save_vals(std::vector<char> vals)
{
	vals_ = vals;
}

void BooleanExpression::save_buffer(std::vector<int> buffer)
{
	buffers.push_back(buffer);
}

static void to_binary(int i, std::vector<int>& buffer, int N)
{
	while (i > 0)
	{
		buffer.push_back(i % 2);
		i = i / 2;
	}

	if (buffer.size() < N)
	{
		while (buffer.size() != N)
		{
			buffer.push_back(0);
		}
	}
}

void push_bool_x(std::stack<std::string>& operands, std::stack<bool>& func_bool, std::vector<int> buffer, int N, int N_initial)
{
	std::string x = operands.top();
	operands.pop();

	bool bool_x = 0;

	if (buffer.empty())
	{
		if (x == "0")
		{
			bool_x = 0;
		}
		else if (x == "1")
		{
			bool_x = 1;
		}
	}
	else
	{
		int idx_x = x[x.size() - 1] - '0';

		// Variable's number (min is at index N, max is at 0) to apply the appropriate bool value from buffer
		int val_x = 0;
		if (idx_x <= N)
		{
			val_x = N - idx_x;
		}
		else  // idx_x > N
		{
			val_x = N_initial - idx_x;
		}

		bool_x = buffer[val_x];

		if (x[0] == '~') { bool_x = !bool_x; };

		if (x == "0")
		{
			bool_x = 0;
		}
		else if (x == "1")
		{
			bool_x = 1;
		}
	}

	func_bool.push(bool_x);
}

std::string BooleanExpression::table()
{
	int N = 0;  // variables amount
	std::vector<char> vals;

	std::stack<char> operators_;
	std::stack<std::string> operands_;

	build_binary_tree(formula_, operators_, operands_, N, vals);

	save_vals(vals);

	int operand_amount = vals.size();

	int N_initial = N;

	// N can be: N <= amount of operands, if N > amount of operands ((~)xi type): N = amount
	// Set up the buffer with initial N value to correctly refer to the buffer for any xi
	if (N > operand_amount)
	{
		N = operand_amount;
	}

	std::string res;

	// Bitmasking to iterate through numbers from 0 to 2^N (1 << N),
	// where 2^N is the amount of possible functions for N variables
	for (int i = 0; i < (1 << N); i++)
	{
		std::stack<char> operators = operators_;
		std::stack<std::string> operands = operands_;

		std::vector<int> buffer;
		to_binary(i, buffer, N);

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
				push_negation(operators, operands);  // => operators.pop()
			}

			std::stack<char> operators_new;
			std::stack<std::string> operands_new;

			int temp1 = 0;
			std::vector<char> vals_temp;
			build_binary_tree(operands.top(), operators_new, operands_new, temp1, vals_temp);
			operands.pop();

			if (operators_new.empty())  // operand didn't need to be unpacked
			{
				push_bool_x(operands_new, func_bool, buffer, N, N_initial);
			}

			while (!operators_new.empty())
			{
				char op_new = operators_new.top();

				if (op_new == ops[0])
				{
					push_negation(operators_new, operands_new);  // => operators_new.pop(), pushed a new (negated) operand to operands_new
				}

				if (operators_new.empty())  // if negation was applied above, push the new negated operand to func_bool
				{
					push_bool_x(operands_new, func_bool, buffer, N, N_initial);
				}
				else
				{
					op_new = operators_new.top();  // negation was part of the operand, take another operator to move forward

					bool bool_x = 0;

					if (!operands_new.empty())
					{
						std::string x = operands_new.top();
						operands_new.pop();

						if (buffer.empty())
						{
							if (x == "0")
							{
								bool_x = 0;
							}
							else if (x == "1")
							{
								bool_x = 1;
							}
						}
						else
						{
							int idx_x = x[x.size() - 1] - '0';

							int val_x = 0;
							if (idx_x <= N)
							{
								val_x = N - idx_x;
							}
							else
							{
								val_x = N_initial - idx_x;
							}

							bool_x = buffer[val_x];

							if (x[0] == '~') { bool_x = !bool_x; };

							if (x == "0")
							{
								bool_x = 0;
							}
							else if (x == "1")
							{
								bool_x = 1;
							}
						}
					}
					else
					{
						bool_x = func_bool.top();
						func_bool.pop();
					}

					bool bool_y = 0;

					if (!operands_new.empty())  // x was popped from operands_new earlier
					{
						std::string y = operands_new.top();
						operands_new.pop();

						if (buffer.empty())
						{
							if (y == "0")
							{
								bool_y = 0;
							}
							else if (y == "1")
							{
								bool_y = 1;
							}
						}
						else
						{
							int idx_y = y[y.size() - 1] - '0';

							int val_y = 0;
							if (idx_y <= N)
							{
								val_y = N - idx_y;
							}
							else 
							{
								val_y = N_initial - idx_y;
							}

							bool_y = buffer[val_y];

							if (y[0] == '~') { bool_y = !bool_y; };

							if (y == "0")
							{
								bool_y = 0;
							}
							else if (y == "1")
							{
								bool_y = 1;
							}
						}
					}
					else
					{
						bool_y = func_bool.top();
						func_bool.pop();
					}

					// The operand is unpacked by build_binary_tree() => op_new is either '&', 'v', '+' or '='
					if (op_new == ops[1])  // '&'
					{
						func_bool.push(bool_x && bool_y);
					}
					else if (op_new == ops[2]) // 'v'
					{
						func_bool.push(bool_x || bool_y);
					}
					else if (op_new == ops[3])  // '+'
					{
						func_bool.push(bool_x ^ bool_y);
					}
					else if (op_new == ops[8])  // '='
					{
						func_bool.push(bool_x == bool_y);
					}

					operators_new.pop();
				}
			}

			if (func_bool.size() > 1)
			{
				if (op != ops[0])  // if op == ops[0] ('~'), operators already popped it
				{
					op = operators.top();
					operators.pop();
				}

				bool x = func_bool.top();
				func_bool.pop();

				bool y = func_bool.top();
				func_bool.pop();

				if (op == ops[1])  // '&'
				{
					func_bool.push(x && y);
				}
				else if (op == ops[2]) // 'v'
				{
					func_bool.push(x || y);
				}
				else if (op == ops[3])  // '+'
				{
					func_bool.push(x ^ y);
				}
				else if (op == ops[4])  // '|'
				{
					func_bool.push(!x || !y);
				}
				else if (op == ops[5])  // '^'
				{
					func_bool.push(!x && !y);
				}
				else if (op == ops[6])  // '<'
				{
					func_bool.push(x || !y);
				}
				else if (op == ops[7])  // '>'
				{
					func_bool.push(!x || y);
				}
				else if (op == ops[8])  // '='
				{
					func_bool.push(x == y);
				}
			}
		}

		// Save buffer (bools of variables) for each bool resulting function for dnf, cnf and zhegalkin
		save_buffer(buffer);

		// Add the bool function value to the truth table
		if (func_bool.empty())
		{
			res.append(operands.top());  // if it is '1' or 0' or xi w/o '~'
		}
		else
		{
			res.push_back(func_bool.top() ? '1' : '0');
		}
	}

	return BooleanExpression(res.c_str());
}

bool BooleanExpression::isFullSystem(const std::vector<BooleanExpression>&)
{
	return false;
}