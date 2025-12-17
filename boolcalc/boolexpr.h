#ifndef BOOLEXPR_H

#include <string>
#include <vector>
#include <stack>
#include <cstring>

/*
		Для представления булевских выражений можно написать класс `BooleanExpression`,
		в котором реализовать :
		- Конструктор от `const char *`, аргументом которого является строка с булевским выражением. При ошибке в выражении конструктор генерирует исключение.
		- Метод `BooleanExpression cnf()` строит конъюнктивную нормальную форму.
		- Метод `BooleanExpression dnf()` строит дизъюнктивную нормальную форму.
		- Метод `BooleanExpression zhegalkin()` — строит полином Жегалкина.
		- Метод `std::string() table()` — строит таблицу истинности.
		- Оператор `operator std::string() const` — формирует строку с булевским выражением.

		Для проверки системы функций на полноту можно реализовать функцию
		bool isFullSystem(const std::vector<BooleanExpression>&);
*/

class BooleanExpression
{
private:
	std::string formula_;

	// Available operators in a descending priority order
	std::vector<char> ops = { '~',  '&', 'v', '+', '|', '^', '<', '>', '=' };

public:
	BooleanExpression(const char* str)
		: formula_(str)
	{
		// ========================================================= Check formula for validity ==============================================================

		std::stack<char> operators;
		std::stack<std::string> operands;

		bool expr = false;

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
				if (s != ops[0])  // will only consider operators between operands, but ops[0] ('~') is a valid operator !!!
				{
					operators.push(s);
				}
			}
			// If the expression is in '()', process it
			else if (s == '(')
			{
				operators.push(s);

				expr = true;  // expression was opened
			}
			else if (s == ')')
			{
				// ================================ Missing '(' bracket ================================
				// Expression was never opened
				if (!expr)
				{
					throw "error";
				}

				char op = operators.top();
				while (op != '(')
				{
					// expression = (operand operator operand) would be pushed to operands as a new operand
					operators.pop();
					operands.pop();

					op = operators.top();
				}
				operators.pop();
				expr = false;  // expression was closed
			}
			// if the symbol is an operand
			else if (s == 'x' || s == '0' || s == '1')
			{
				std::string op1;
				op1.push_back(s);

				if (s != '0' && s != '1')
				{
					char s1 = formula_[++i];

					// ================================ Incorrect operand input ================================
					if (std::isdigit(s1) == 0)
					{
						throw "error";
					}

					op1.push_back(s1);
				}

				operands.push(op1);

				// ============================== Missing operators between operands + Missing ')' bracket ==============================
				if (operands.size() - 1 != operators.size() - static_cast<unsigned long>(expr))  // if '(' is in operators, it will not be included in operators' size
				{
					throw "error";
				}
			}
			// ================================ Unknown symbols ================================
			else
			{
				throw "error";
			}

			++i;
		}

		// ======================== If left in the end operands and operators don't match accordingly
		//                          + Missing ')' bracket (expression was never closed) ========================
		if (!operands.empty() && !operators.empty() && operands.size() - 1 != operators.size())
		{
			throw "error";
		}
	}

	// De Morgan's law
	std::string de_morgan_law(std::string x);

	void push_negation(std::stack<char>& operators, std::stack<std::string>& operands);
	void push_operand(std::stack<char>& operators, std::stack<std::string>& operands, char op);

	std::vector<char> vals_;
	void build_binary_tree(std::string formula, std::stack<char>& operators, std::stack<std::string>& operands, int& N, std::vector<char>& vals);
	void save_vals(std::vector<char> vals);

	BooleanExpression cnf();
	BooleanExpression dnf();

	bool is_not_lineal = false;  // for a function

	// Constructed by a triangle method
	BooleanExpression zhegalkin(); 

	std::vector<std::vector<int>> buffers;
	void save_buffer(const std::vector<int>& buffer);

	void table_helper(std::stack<std::string>& operands, std::stack<bool>& func_bool, const std::vector<int>& buffer, int N, int N_initial);
	std::string table();

	operator std::string() const 
	{
		return formula_;
	}

	static bool isFullSystem(const std::vector<BooleanExpression>& system)
	{
		// The system is full when at least one function doesn't belong to one of the 5 classes, listed below
		bool is_full_system = false;

		bool classes[5];
		std::memset(classes, 1, 5);

		for (size_t i = 0; i < system.size(); i++)
		{
			BooleanExpression func = system[i];

			std::string res = func.table();  // => std::vector<std::vector<int>> buffers

			// 1) f(0) != 0
			if (classes[0] && res[0] != '0')
			{
				classes[0] = false;
			}

			// 2) f(1) != 1
			if (classes[1] && res[res.size() - 1] != '1')
			{
				classes[1] = false;
			}

			// 3) Function is not lineal
			//    => Zhegalkin polnomial contains conjunctions 
			//    => the polynomial's degree is greater than one

			// Calling func.zhegalkin() would create a copy of func anyway, BUT then we would check is_not_lineal of original func,
			// which is false by default, before calling zhegalkin method
			BooleanExpression func_copy = func;
			func_copy.zhegalkin();

			if (classes[2] && func_copy.is_not_lineal)
			{
				classes[2] = false;
			}

			// 4) Function is not monotonic

			// buffers: 00..00, 00..01, .., 10..00, .., 11..11 => the function is non-decreasing left to right of res (truth table)

			bool is_not_monotonic = false;

			for (size_t j = 0; j < res.size() - 1; j++)
			{
				// Function is decreasing 
				if (res[j] > res[j + 1])
				{
					is_not_monotonic = true;
				}

				if (is_not_monotonic)
				{
					if (classes[3])
					{
						classes[3] = false;
					}
					break;
				}
			}

			// 5) Function is not self-dual
			std::string reversed = res;
			std::reverse(reversed.begin(), reversed.end());

			if (classes[4] && res != reversed)
			{
				classes[4] = false;
			}
		}

		int sum = 0;
		for (bool c : classes)
		{
			sum += static_cast<int>(c);
		}

		if (sum == 0)
		{
			is_full_system = true;
		}

		return is_full_system;
	}
};

#endif