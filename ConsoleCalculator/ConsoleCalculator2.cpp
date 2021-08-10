#include <iostream>
#include <string>
#include <iterator>
#include "../../MathFunctions.cpp"
#include <windows.h>
using namespace std;
bool debug = 0;	//	отладка
complex ans, calc_res = $zero + 1;
int index = 0;
string f[16];
//	функции поиска символов
bool find_x(string str) {
	for (int i = 0; i < str.length(); i++)
		if (str.substr(i, 3) == "exp")
			i = i + 2;
		else if (str[i] == 'x')	return true;
	return false;
}
int log_bracket(string str);

// возвращает номер закрывающей скобки для данной открывающей ( )
int bracket(string str) {
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == '[')	i = log_bracket(str.substr(i, str.length()))++;
		if (str[i] == '(')	i = bracket(str.substr(i, str.length()))++;
		if (str[i] == ')')	return i;
	}
	return 1;
}
// возвращает номер закрывающей скобки для данной открывающей [ ]
int log_bracket(string str) {
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == '[')	i = log_bracket(str.substr(i))++;
		if (str[i] == '(')	i = bracket(str.substr(i))++;
		if (str[i] == ']')	return i;
	}
	return str.length();
}
// возвращает номер перед +, -, ) или ]
int first_0_priority(string str) {
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == '[')	i = log_bracket(str.substr(i))++;
		if (str[i] == '(')	i = bracket(str.substr(i))++;
		if ((str[i] == '+') ||
			(str[i] == '-') ||
			(str[i] == ')') ||
			(str[i] == ']'))	return i--;
	}
	return str.length()--;
}
// возвращает номер перед +, -, *, /, ) или ]
int first_1_priority(string str) {
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == '[')	i = log_bracket(str.substr(i))++;
		if (str[i] == '(')	i = bracket(str.substr(i))++;
		if ((str[i] == ' ') ||
			(str[i] == '+') ||
			(str[i] == '-') ||
			(str[i] == '*') ||
			(str[i] == '/') ||
			(str[i] == ')') ||
			(str[i] == ']'))	return i--;
	}
	return str.length()--;
}

//	вычислитель
complex calcccccc(object obj[], int first, int last, complex x, complex n) {
	if (debug) {
		cout << "\ncalc[" << first << " ; " << last << "][x = ";
		print_c(x); 
		cout << "][n = ";
		print_c(n);
		cout << "][ans = ";
		print_c(ans);
		cout << "]";
		for (int j = 0; j <= last; j++) {
			if (j == (first)) cout << "\n-------";
			if (obj[j].type == 0) {
				cout << '\n' << j << "  ";
				print_c(obj[j].value);
				continue;
			}
			if (obj[j].type == 3) cout << '\n' << j << "  x";
			else cout << '\n' << j << "  " << obj[j].content;
		}
	}
	if (first < last) {
		if ((obj[first].type != 0) && 
			(obj[first].type != 3) && 
			(obj[first].type != 4) && 
			(obj[first].content == '\0')) {
			if (debug) cout << "\n[x]";
			return calc(obj, first + 1, last, x, n);
		}
		int c;
		if ((obj[first].type == 0) || 
			(obj[first].type == 3) || 
			(obj[first].type == 4)) {
			if (obj[first + 1].type == 1) {
				if (obj[first + 1].content == '!') {
					obj[first + 1].value = fct(value(obj[first], x, n));
					obj[first + 1].type = 0;
					return calc(obj, first + 1, last, x, n);
				}
				if (obj[first + 1].content == '^') {
					c = bracket(obj, first + 2, last);
					if (debug) cout << "\nbracket = " << c;
					c = first_1_priority(obj, c, last);
					if (debug) cout << "\nfirst_1 = " << c;
					obj[c].value = power(value(obj[first], x, n), calc(obj, first + 2, c, x, n));
					obj[c].type = 0;
					return calc(obj, c, last, x, n);
				}
				if (obj[first + 1].content == '/') {
					c = first_1_priority(obj, first + 1, last);
					if (debug) cout << "\nfirst_1 = " << c;
					obj[c].value = value(obj[first], x, n) / calc(obj, first + 2, c, x, n);
					obj[c].type = 0;
					return calc(obj, c, last, x, n);
				}
				c = first_0_priority(obj, first + 2, last);
				if (debug) cout << "\nfirst_0 = " << c;
				obj[c].value = value(obj[first], x, n) + calc(obj, first + 1, c, x, n);
				obj[c].type = 0;
				return calc(obj, c, last, x, n);
			}
			c = first_1_priority(obj, first + 1, last);
			if (debug) cout << "\nfirst_1 = " << c;
			obj[c].value = value(obj[first], x, n) * calc(obj, first + 1, c, x, n);
			obj[c].type = 0;
			return calc(obj, c, last, x, n);
		}
		if (obj[first].content == '-') {
			c = first_1_priority(obj, first + 1, last);
			if (debug) cout << "\nfirst_1 = " << c;
			complex _res = calc(obj, first + 1, c, x, n);
			obj[c].value.R = -_res.R;
			obj[c].value.i = -_res.i;
			obj[c].type = 0;
			return calc(obj, c, last, x, n);
		}
		if (obj[first].type == 1) return calc(obj, first + 1, last, x, n);
		if (obj[first].content == '(') {
			c = bracket(obj, first, last);
			if (debug) cout << "\nb () = " << c;
			obj[c].value = calc(obj, first + 1, c - 1, x, n);
			obj[c].type = 0;
			return calc(obj, c, last, x, n);
		}
		c = bracket(obj, first + 1, last);
		if (debug) cout << "\nbracket = " << c;
		bool re = 0;
		switch (obj[first].content) {
		case('a'): {
			obj[first].value = ans;
			obj[first].type = 0;
			return calc(obj, first, last, x, n);
			break;
		}
		case('m'): {
			obj[c].value = $constant + calc(obj, first + 1, c, x, n).mod();
			re = true;
			break;
		}

		case('c'): {
			obj[c].value = cos( calc(obj, first + 1, c, x, n) );
			re = true;
			break;
		}
		case('s'): {
			obj[c].value = cos(calc(obj, first + 1, c, x, n) - pi / 2);
			re = true;
			break;
		}
		case('t'): {
			obj[c].value = cos(calc(obj, first + 1, c, x, n) - pi / 2) / cos(calc(obj, first + 1, c, x, n));
			re = true;
			break;
		}
		case('g'): {
			obj[c].value = cos(calc(obj, first + 1, c, x, n)) / cos(calc(obj, first + 1, c, x, n) - pi / 2);
			re = true;
			break;
		}

		case('1'): {
			obj[c].value = arccos( calc(obj, first + 1, c, x, n) );
			re = true;
			break;
		}
		case('2'): {
			obj[c].value = pi / 2 - arccos( calc(obj, first + 1, c, x, n) );
			re = true;
			break;
		}
		case('3'): {
			complex w = calc(obj, first + 1, c, x, n);
			obj[c].value = D(w.R) * arccos( 1 / power(1 + w * w, 0.5) );
			re = true;
			break;
		}

		case('R'): {
			obj[c].value = calc(obj, first + 1, c, x, n);
			obj[c].value.i = 0;
			re = true;
			break;
		}
		case('I'): {
			obj[c].value = calc(obj, first + 1, c, x, n);
			obj[c].value.R = 0;
			re = true;
			break;
		}

		case('l'): {
			obj[c].value = ln(calc(obj, first + 1, c, x, n));
			re = true;
			break;
		}
		case('e'): {
			obj[c].value = exp(calc(obj, first + 1, c, x, n));
			re = true;
			break;
		}
		case('r'): {
			obj[c].value = power(calc(obj, first + 1, c, x, n), 0.5);
			re = true;
			break;
		}
		}
		if (re) {
			obj[c].type = 0;
			return calc(obj, c, last, x, n);
		}
		int p = log_bracket(obj, first + 1, last);
		c = bracket(obj, p + 1, last);
		if (debug) {
			cout << "\nbracket = " << c;
			cout << "\nlog_bracket = " << p;
		}
		if  (obj[first].content == '[') obj[c].value = log( calc(obj, first + 1, p - 1, x, n), calc(obj, p + 1, c, x, n));
		obj[c].type = 0;
		return calc(obj, c, last, x, n);
	}
	if (obj[first].content == 'a') return ans;
	return value(obj[first], x, n);
}
complex calc(string str, complex x) {
	if (debug) {
		cout << "calc(" << str << ")";
	}
	int end = 0;
	if ((48 <= (int)str[0]) && ((int)str[0] <= 57)) {
		bool dot = false;
		int k = 1;
		calc_res = $zero + (int)str[0] - 48;
		while ((48 <= (int)str[k]) && ((int)str[k] <= 57)) {

		}
	}
	end = bracket(str.substr(6));
	switch (str.substr(0, 6)) {
	case("arccos"): {
		calc_res = arccos(calc(str.substr(6, end)));	//	calc(str.substr(end))
		break;
	}
	case("arcsin"): {
		calc_res = pi / 2 - arccos(calc(str.substr(6, end)));
		break;
	}
	case("arctan"): {
		break;
	}
	default:switch (str.substr(0, 4)) {
	case("sqrt"): {
		break;
	}
	case("log["): {
		break;
	}
	case("real"): {
		break;
	}
	default:switch (str.substr(0, 3)) {
	case("img"): {
		break;
	}
	case("sin"): {
		break;
	}
	case("cos"): {
		break;
	}
	case("exp"): {
		break;
	}
	case("tan"): {
		break;
	}
	case("cot"): {
		break;
	}
	case("ans"): {
		break;
	}
	case("abs"): {
		break;
	}
	case("inf"): {
		break;
	}
	default:switch (str.substr(0, 2)) {
	case("ln"): {
		break;
	}
	case("pi"): {
		break;
	}
	default:
	}
	}
	}
	}
}

//	парсер
void res(string x) {
	int l = x.length(), c = -1, g = 0;
	bool dot = 0;
	if (l > 128) {
		cout << "length of string is over than 128" << endl;
		return;
	}
	object obj[128],graph[128];
	for (int i = 0; i < l; i++) {
		c++;
		if ((48 <= (int)x[i]) && ((int)x[i] <= 57)) {
			obj[c].type = 0;
			if (c == 0) {
				obj[c].value.R = (double)x[i] - 48;
				continue;
			}
			if (dot) {
				c--;
				g++;
				for (int j = 0; j < g; j++) obj[c].value.R = obj[c].value.R * 10;
				obj[c].value.R = obj[c].value.R + ((double)x[i] - 48);
				for (int j = 0; j < g; j++) obj[c].value.R = obj[c].value.R / 10;
				continue;
			}
			if ((47 < (int)x[i - 1]) && ((int)x[i - 1] < 58)) c--;
			obj[c].value.R = obj[c].value.R * 10 + (double)((double)x[i] - 48);
			continue;
		}
		g = 0;
		dot = 0;
		obj[c].type = 2;
		if		(x.substr(i, 6) == "arccos")obj[c].content = '1';	//	arccos
		else if (x.substr(i, 6) == "arcsin")obj[c].content = '2';	//	arcsin
		else if (x.substr(i, 6) == "arctan")obj[c].content = '3';	//	arctan
		else if (x.substr(i, 4) == "sqrt")	obj[c].content = 'r'; 	//	sqrt
		else if (x.substr(i, 4) == "log[")	obj[c].content = '[';	//	log[
		else if (x.substr(i, 4) == "real")	obj[c].content = 'R';	//	real .R
		else if (x.substr(i, 3) == "img")	obj[c].content = 'I';	//	img  .i
		else if (x.substr(i, 3) == "sin")	obj[c].content = 's';	//	sin
		else if (x.substr(i, 3) == "cos")	obj[c].content = 'c';	//	cos
		else if (x.substr(i, 3) == "exp")	obj[c].content = 'e';	//	exp
		else if (x.substr(i, 3) == "tan")	obj[c].content = 't';	//	tan
		else if (x.substr(i, 3) == "cot")	obj[c].content = 'g';	//	cot
		else if (x.substr(i, 3) == "ans")	obj[c].content = 'a';	//	ans
		else if (x.substr(i, 3) == "abs")	obj[c].content = 'm';	//	abs
		else if (x.substr(i, 3) == "inf") {								//	inf
			obj[c].value.type = 1;
			i = i + 2;
			continue;
		}
		else if (x.substr(i, 2) == "ln")	obj[c].content = 'l';	//	ln
		else if (x.substr(i, 2) == "pi") {
			obj[c].type = 0;
			obj[c].value.R = pi;
			i++;
			continue;
		}
		switch (obj[c].content) {								//	сдвиг символа
		case('1'):case('2'):case('3'): {
			i = i + 5;
			continue;
		}
		case('r'):case('['):case('R'): {
			i = i + 3;
			continue;
		}
		case('s'):case('c'):case('e'):case('t'):case('g'):case('a'):case('m'):case('I'): {
			i = i + 2;
			continue;
		}
		case('l'): {
			i++;
			continue;
		}
		}
		switch (x[i]) {
		case(' '): {
			c--;
			break;
		}
		case('*'): break;
		case('.'):case(','): {
			dot = 1;
			if (i > 0) if ((47 < (int)x[i - 1]) && ((int)x[i - 1] < 58)) c--;
			break;
		}
		case('+'):case('-'):case('/'):case('^'):case('!'): {
			obj[c].type = 1;
			obj[c].content = x[i];
			break;
		}
		case('('):case(')'):case(']'): {
			obj[c].content = x[i];
			break;
		}
		case('x'): {
			obj[c].type = 3;
			break;
		}
		case('e'): {
			obj[c].type = 0;
			obj[c].value.R = e;
			break;
		}
		case('i'): {
			if (i == 0) {
				obj[c].type = 0;
				obj[c].value.i = 1;
				break;
			}
			if ((47 < (int)x[i - 1]) && ((int)x[i - 1] < 58)) {
				c--;
				obj[c].type = 0;
				obj[c].value.i = obj[c].value.R;
				obj[c].value.R = 0;
				break;
			}
			obj[c].type = 0;
			obj[c].value.i = 1;
			break;
		}
		}
	}
	if (debug) {
		cout << "\nobj[" << 0 << " ; " << c << "]";
		for (int j = 0; j <= c; j++) {
			if (obj[j].type == 3) {
				cout << '\n' << j << "  x";
				continue;
			}
			if (obj[j].type == 0) {
				cout << '\n' << j << "  ";
				print_c(obj[j].value);
				continue;
			}
			cout << '\n' << j << "  " << obj[j].content;
		}
	}
	if (find_x(obj, c)) {
		f[index] = x;
		if (index < 15) index++;
		else			index = 0;
		contin = true;
		if (x_resolution < 0) x_resolution = -x_resolution;
		if (y_resolution < 0) y_resolution = -y_resolution;
		complex x, y0;
		int yi = 0, yi2 = 0;
		for (int i = 0; i <= 96; i++) {
			x.R = x_min + (double)i * x_resolution;
			for (int d = 0; d <= c; d++) graph[d] = obj[d];
			complex function = calc(graph, 0, c, x, n);
			if ((function.i == 0) && (function.type == 0)) {
				for (int y = 0; y < 49; y++) {
					//if ((function.R < y_min) || (y_max < function.R)) continue;
					double m = y_min + (double)y * y_resolution;
					if ((m <= function.R) && (function.R <= (m + 0.333333 * y_resolution))) {
						yi2 = y;
						matrix[i][y] = ',';
					}
					else if (((m + 0.333333 * y_resolution) < function.R) && (function.R < (m + 0.666667 * y_resolution))) {
						yi2 = y;
						matrix[i][y] = '-';
					}
					else if (((m + 0.666667 * y_resolution) <= function.R) && (function.R < (m + y_resolution))) {
						yi2 = y;
						matrix[i][y] = '"';
					}
					else {
						if ((y0.i == 0) && (y0.type == 0) &&
							(function.R < infinity) && (-infinity < function.R)
							&& (i > 0) && (y != yi) &&
							(((y0.R < m) && (m < function.R)) || ((m < y0.R) && (function.R < m))))			matrix[i][y] = ':';
					}
				}
			}
			y0 = function;
			yi = yi2;
		}
	}
	else {
		contin = false;
		complex result = calc(obj, 0, c, $constant, $constant);
		//if ((-0.0001 < result.R) && (result.R < 0.0001)) result.R = 0;				//	округление маленьких значений
		//if ((-0.0001 < result.i) && (result.i < 0.0001)) result.i = 0;
		ans = result;
		cout << "	";
		if (result.type == 0) print_c(result);
		if (result.type == 1) cout << "inf";
		if (result.type == 2) cout << "NaN";
		if (result.type == 3) cout << "unknown";
		cout << endl << endl;
	}
}