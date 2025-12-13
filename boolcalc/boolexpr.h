#pragma once
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
				while ((op = operators.top()) != '(')
				{
					// expression = (operand operator operand) would be pushed to operands as a new operand
					operators.pop();
					operands.pop();
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
					if (!std::isdigit(s1))
					{
						throw "error";
					}

					op1.push_back(s1);
				}

				operands.push(op1);

				// ============================== Missing operators between operands + Missing ')' bracket ==============================
				if (operands.size() - 1 != operators.size() - expr)  // if '(' is in operators, it will not be included in operators' size
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

		// ======================== If left operands and operators don't match accordingly + Missing ')' bracket (expression was never closed) ========================
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
	void save_buffer(std::vector<int> buffer);

	std::string table();

	operator std::string() const 
	{
		return formula_;
	}

	bool isFullSystem(const std::vector<BooleanExpression>& system);
};