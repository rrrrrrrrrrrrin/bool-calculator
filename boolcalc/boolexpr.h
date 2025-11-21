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
	const char* str_;

public:
	BooleanExpression(const char* str)
		: str_(str)
	{
		// Check str for validity
		// ~ can be before xN and one of the logical operations between any xi, xj 
		std::vector<char> operators = { '~', '&', 'v', '+', '>', '<', '=', '|', '^'};
		char x = 'x';

		int i = 0;
		while (str[i] != '\0')
		{
			char s = str[i];
			if (s == operators[0] && (str[i + 1] == '\0' || str[i + 1] != x))
			{
				throw "error";
			}
			else if (s == x && (str[i + 2]) == '\0' || std::find(operators.begin(), operators.end(), str[i + 2]) == operators.end() 
				                  || str[i+4] == '\0' || str[i+4] != x)
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

	}

	bool isFullSystem(const std::vector<BooleanExpression>&);
};