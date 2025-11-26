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

	char bracket = ' ';

	while (str >> c)
	{
		str_for_next_word >> y;  // read current word from another buffer
		
		// if new word is an expression, save and skip the current operator, save dnf, process the new word,
		// then process new word, saved dnf and operator 
		if (y[0] == '(')  // can't be the first expression as it is next word
		{

		}

		// Expression can start with '(', trim c and save brackets to work with when the operator is parsed
		if (c[0] == '(')
		{
			bracket = c[0];
			c.erase(c.begin(), c.begin() + 1);  // s = xN (~xN)
		}

		// 1) Избавиться от всех логических операций, содержащихся в формуле, 
		//    заменив их основными: конъюнкцией, дизъюнкцией, отрицанием

		char op = c[0];
		// If s is an operator, read next word
		if (std::find(ops.begin() + 1, ops.end(), c[0]) != ops.end())
		{
			str_for_next_word >> y;  // read next word from another buffer. can end in ')'

			// Delete previous variable or expression from dnf to apply operator on it and next word
			if (!dnf.empty())
			{
				size_t idx1 = dnf.rfind(' ');
				dnf.erase(dnf.begin() + idx1, dnf.end());

				// if previous variable ends in ')', previous variable is the expression in brackets
				if (x[x.size() - 1] == ')')
				{
					size_t idx2 = dnf.rfind('(');  // find the last occurrence of '(' from right to left
					dnf.erase(dnf.begin() + idx2, dnf.end());
				}

				dnf += ' ';
			}

			// If the previous variable started with a '(', start an expression
			if (bracket == '(')
			{
				dnf += bracket;
				bracket = ' ';
			}

			// TODO: В конъюнктах и дизъюнктах переменные записываются по возрастанию их индексов

			// No changes
			if (op == ops[1] || op == ops[2])  // '&' or 'v'
			{
				dnf += x + ' ' + op + ' ' + y;
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

			str >> c;  // skip next word in the main buffer str
		}

		if (dnf.empty()) { x = c; }  // save current word as previous word for the next iteration 

		if (!dnf.empty())
		{
			std::string dnf_temp = dnf;

			// get previous variable from dnf
			size_t idx1 = dnf_temp.rfind(' ') + 1;
			x = dnf_temp.substr(idx1, dnf_temp.size() - idx1);

			// if previous variable ends in ')', previous variable becomes the expression in brackets
			if (x[x.size() - 1] == ')')
			{
				size_t idx2 = dnf_temp.rfind('(');  // find the last occurrence of '(' from right to left
				x = dnf_temp.substr(idx2, dnf_temp.size() - idx2);
			}
		}

		//// if next variable doesn't end in ')', previous variable becomes the last variable of dnf
		//if (!dnf.empty() && y[y.size() - 1] != ')')
		//{
		//	dnf_temp.erase(dnf.end() - 3, dnf.end());
		//	x = dnf_temp;
		//}
	}

	// 2) Заменить знак отрицания, относящийся ко всему выражению, знаками отрицания, 
	//    относящимися к отдельным переменным, высказываниям, на основании закона Де Моргана 
	//    Например, ~(x1 v x2) -> ~x1 & ~x2

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