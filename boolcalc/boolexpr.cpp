#include "boolexpr.h"
#include <sstream>

BooleanExpression BooleanExpression::cnf()
{
	return BooleanExpression("pass");
}

BooleanExpression BooleanExpression::dnf()
{
	// std::vector<char> operators = 
	// { '~', '&', 'v', '+', '>', '<', '=', '|', '^' };

	std::stringstream str(formula_);
	std::stringstream str_for_next_word(formula_);

	std::string dnf;

	std::string x;  // previous word
	std::string c;  // current word
	std::string y;  // next word

	while (str >> c)
	{
		str_for_next_word >> y;  // read current word from another buffer

		char bracket = ' ';

		// Expression can start (end) with '(', ( ')' ), trim s and save brackets
		if (c[0] == '(')
		{
			bracket = c[0];
			c.erase(c.begin(), c.begin() + 1);  // s = xN (~xN)
		}

		if (c[c.size() - 1] == ')')
		{
			bracket = c[c.size() - 1];
			c.erase(c.end() - 1, c.end()); 
		}

		// 1) Избавиться от всех логических операций, содержащихся в формуле, 
		//    заменив их основными: конъюнкцией, дизъюнкцией, отрицанием

		char op = c[0];
		// If s is an operator, read next word
		if (std::find(ops.begin() + 3, ops.end(), c[0]) != ops.end())
		{
			str_for_next_word >> y;  // read next word from another buffer

			if (bracket == '(')
			{
				dnf += bracket;
			}

			// No changes
			if (op == ops[1] || op == ops[2])  // '&' or 'v'
			{
				dnf += x + op + y;
			}

			if (op == ops[3])  // '+'
			{
				dnf += x + " & " + '~' + y + " v " + '~' + x + " & " + y;
			}
			else if (op == ops[4])  // '>'
			{
				dnf += '~' + x + " v " + y;
			}
			else if (op == ops[5])  // '<'
			{
				dnf += x + " v " + '~' + y;
			}
			else if (op == ops[6])  // '='
			{
				dnf += "(~" + x + " & " + '~' + y + ')' + " v " + '(' + x + " & " + y + ')';
			}
			else if (op == ops[7])  // '|'
			{
				dnf += '~' + x + " v " + '~' + y;
			}
			else if (op == ops[8])  // '^'
			{
				dnf += '~' + x + " & " + '~' + y;
			}

			dnf += ' ';

			str >> c;  // skip next word in the main buffer str
		}

		if (bracket == ')')
		{
			dnf += bracket;
		}

		x = c;  // save current word as last word for the next iteration 
	}

	// 2) Заменить знак отрицания, относящийся ко всему выражению, знаками отрицания, 
	//    относящимися к отдельным переменным, высказываниям, на основании закона Де Моргана

	// 3) Избавиться от знаков двойного отрицания (инволютивный закон)

	// 4) Применить, если нужно, к операциям конъюнкции и дизъюнкции
	//    свойства дистрибутивности и законы поглощения, идемпотентности
	//    чтобы привести к ДНФ или КНФ

	BooleanExpression result_dnf(dnf.c_str()+'\0');
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