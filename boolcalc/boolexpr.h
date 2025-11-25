#pragma once
#include <string>
#include <vector>

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
	const char* formula_;

	std::vector<char> ops = { '~', '&', 'v', '+', '>', '<', '=', '|', '^' };  // operators

public:
	BooleanExpression(const char* str)
		: formula_(str)
	{
		// Check formula for validity

		std::string temp;
		temp += str;

		int str_size = temp.size() - 1;

		int i = 0;
		while (i <= str_size)
		{
			char s = str[i];
			char x = 'x';

			// Formula is at '(', check for x or '~' afterwards
			if ( s == '(' && str[i + 1] != '~' && str[i + 1] != x )
			{
				throw "error";
			}
			// Formula is at '~', check for x afterwards
			else if ( (s == '~') && (i + 1) == str_size && str[i + 1] != x )
			{
				throw "error";
			} 


			int i2 = (i + 2) > str_size ? str_size : i + 2;
			int i3 = (i + 3) > str_size ? str_size : i + 3;
			int i5 = (i + 5) > str_size ? str_size : i + 5;

			// Formula is at x, check if the operator afterwards is valid (if not the end of the formula), 
			//                  and check if there is another x after the operator 
			//                  and check if the expression is closed with ')' (if no operator afterwards or end)
			if (s == x  && i3 != str_size && std::find(ops.begin() + 1, ops.end(), str[i3]) == ops.end()
				        && i5 != str_size && str[i5] != x
				        && i3 != str_size && std::find(ops.begin(), ops.end(), str[i3]) == ops.end()
				        && str[i2] != ')')
			{
				throw "error";
			}
			++i;
		}
	}

	BooleanExpression cnf();
	BooleanExpression dnf();
	BooleanExpression zhegalkin();

	std::string table();

	operator std::string() const 
	{
		// std::string result_str(formula_); 

		std::string result_str;
		result_str += formula_;

		return result_str;
	}

	bool isFullSystem(const std::vector<BooleanExpression>&);
};