#pragma once
#include <string>
#include <vector>
#include <stack>

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
		// Check formula for validity

		/*for (size_t i = 0; i < formula_.length(); i++)
		{

		}*/
		
	}

	// De Morgan's law
	std::string de_morgan_law(std::string x);

	void push_negation(std::stack<char>& operators, std::stack<std::string>& operands);
	void push_operand(std::stack<char>& operators, std::stack<std::string>& operands, char op);
	void new_operand(std::stack<char> &operators, std::stack<std::string> &operands, char op_previous, char op_current);

	void build_binary_tree(std::string formula, std::stack<char>& operators, std::stack<std::string>& operands, int& N, int& operand_amount);

	BooleanExpression cnf();
	BooleanExpression dnf();
	BooleanExpression zhegalkin();

	std::string table();

	operator std::string() const 
	{
		return formula_;
	}

	bool isFullSystem(const std::vector<BooleanExpression>&);
};