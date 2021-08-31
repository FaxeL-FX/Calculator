#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../MathFunctions.cpp"
#include "BMPWriter.h"
using namespace std;
bool debug = 0, rounded = 0, axis = 1, grid = 1, visual = 0, hyperbolic = 0, render = 1, extends = 0;
const string ver = "	v 1.7.5\n";
string inst;
complex ans, Cnum;

struct object {
	int			 type = 0;
	char	  content = '\0';
	complex		value;
	object() {}
	object(int empty) {}
	object(int type, double real) {
		this->type = type;
		this->value.R = real;
	}
	object(int type, complex value) {
		this->type = type;
		this->value = value;
	}
	object(int type, char content) {
		this->type = type;
		this->content = content;
	}
};
struct graph {
	string name;
	vector<object> fnc;
	int type = 0,
		equal_index = 0;
	bool show = true;
	graph(string nm, vector<object> f, int index) {
		name = nm;
		fnc = f;
		equal_index = index;
	}
	graph() {}
};
vector<graph> f, eq, pl;

int x_size = 64, y_size = 32; // 96 48
double x_min = -4, x_max = 4, y_min = -4, y_max = 4;
double x_resolution = (x_max - x_min) / x_size, y_resolution = (y_max - y_min) / y_size,
		mid_resolution = exp(ln(x_resolution * y_resolution) / 2);
double res025 = 0.25 * mid_resolution,
	    res05 = 0.5 * mid_resolution,
	     res2 = 2 * mid_resolution,
	     res4 = 4 * mid_resolution,
	     res8 = 8 * mid_resolution,
	    res27 = 27 * mid_resolution,
	   res256 = 256 * mid_resolution,
	  res3125 = 3125 * mid_resolution,
	 res46656 = 46656 * mid_resolution,
	res823543 = 823543 * mid_resolution;
int matrix[201][101];

bool command(string com);
void clear_matrix();
void draw();

int bracket(vector<object> obj, int main, int i_max);
int log_bracket(vector<object> obj, int i, int i_max);

void pen(int x, int y, int pixel) {
	if (matrix[x][y] < pixel) {
		if (matrix[x][y] >= 0)	matrix[x][y] = pixel;
		else if (pixel > 2)		matrix[x][y] = pixel;
	}
}
float* pen(float* img, int width, int heigth, int x, int y, double pixel) {
	int position = (heigth - y - 1) * width + x;
	if ((0 <= position) && (position < width * heigth)) {
		if ((0 <= x) && (x < width))
			if ((0 <= y) && (y < heigth))
				img[position] = pixel;
	}
	return img;
}
float* penAdd(float* img, int width, int heigth, int x, int y, double pixel) {
	int position = y * width + x;
	if ((0 <= position) && (position < width * heigth)) {
		if ((0 <= x) && (x < width))
			if ((0 <= y) && (y < heigth))
				img[position] = 1 + ((img[position] - 1) * (1 - pixel));
	}
	return img;
}
void print_c(complex x) {
	if (x.mod() == $inf) {
		cout << "inf";
		return;
	}
	if ((x.R == 0) && (x.i == 0)) {
		cout << "0";
		return;
	}
	if (x.R != 0)		cout << x.R;
	if ((x.i > 0) &&
		(x.R != 0))		cout << '+';
	if ((x.i != -1) &&
		(x.i != 0) &&
		(x.i != 1))		cout << x.i;
	else if (x.i == -1) cout << '-';
	if (x.i != 0)		cout << 'i';
}
graph parse(string s) {
	int g = 0, equal_index = 0, type = 0;
	bool dot = 0;
	graph NewGraph;
	NewGraph.name = s;
	s = '(' + s + ')';
	vector<object> obj;
	object babka;
	for (int i = 0; i < s.length(); i++) {
		babka.content = '\0';
		babka.value = 0;
		if ((48 <= (int)s[i]) && ((int)s[i] <= 57)) {
			babka.type = 0;
			if (dot) {
				babka.value.R = (double)s[i] - 48;
				g++;
				for (int j = 0; j < g; j++) babka.value.R = babka.value.R / 10;
				obj.back().value.R += babka.value.R;
				continue;
			}
			if (!obj.empty()) {
				if ((47 < (int)s[i - 1]) && ((int)s[i - 1] < 58)) {
					babka.value.R = obj.back().value.R * 10 + ((double)s[i] - 48);
					obj.back() = babka;
					continue;
				}
			}
			babka.value.R = (double)s[i] - 48;
			obj.push_back(babka);
			continue;
		}
		g = 0;
		dot = 0;
		babka.type = 2;
			 if	(s.substr(i, 9) == "TaylorExp") babka.content = '1';
		else if (s.substr(i, 8) == "LimitExp")	babka.content = '2';

		else if (s.substr(i, 6) == "arccos")	babka.content = 'c';	//	arccos
		else if (s.substr(i, 6) == "arcsin")	babka.content = 's';	//	arcsin
		else if (s.substr(i, 6) == "arctan")	babka.content = 't';	//	arctan
		else if (s.substr(i, 6) == "arccot")	babka.content = 'g';	//	arccot
		else if (s.substr(i, 6) == "arcctg")	babka.content = 'g';	//	arcctg (arccot)
		else if (s.substr(i, 6) == "arccyc")	babka.content = 'd';	//	arccyc
		else if (s.substr(i, 6) == "existG")	babka.content = '|';	//	arccyc
		else if (s.substr(i, 6) == "cycle:") {
			if (type == 0) type = 1;
			babka.content = ':';	//	cycle
		}

		else if (s.substr(i, 5) == "arctg") {						//	arctg  (arctan)
			babka.content = 't';
			i = i + 4;
			obj.push_back(babka);
			continue;
		}
		else if (s.substr(i, 5) == "root[")		babka.content = 'r';	//	real .R
		else if (s.substr(i, 5) == "exist")		babka.content = '=';	//	exist
		else if (s.substr(i, 5) == "gamma")		babka.content = '!';	//	gamma

		else if (s.substr(i, 4) == "sqrt")		babka.content = 'q'; 	//	sqrt
		else if (s.substr(i, 4) == "log[")		babka.content = '[';	//	log[
		else if (s.substr(i, 4) == "cosh")		babka.content = '5';	//	cosh ->    cos(xi)
		else if (s.substr(i, 4) == "sinh")		babka.content = '6';	//	sinh -> -i sin(xi)
		else if (s.substr(i, 4) == "zeta")		babka.content = 'z';	//	zeta
		else if (s.substr(i, 4) == "grid")		babka.content = '+';	//	grid

		else if (s.substr(i, 3) == "sin")		babka.content = 'S';	//	sin
		else if (s.substr(i, 3) == "cos")		babka.content = 'C';	//	cos
		else if (s.substr(i, 3) == "exp")		babka.content = 'e';	//	exp
		else if (s.substr(i, 3) == "tan")		babka.content = 'T';	//	tan
		else if (s.substr(i, 3) == "cot")		babka.content = 'G';	//	cot
		else if (s.substr(i, 3) == "ctg")		babka.content = 'G';	//	ctg (cot)
		else if (s.substr(i, 3) == "cyc")		babka.content = 'D';	//	cyc
		else if (s.substr(i, 3) == "ans") {
			babka.type = 3;
			babka.content = 'a';	//	ans
			i = i + 2;
			obj.push_back(babka);
			continue;
		}
		else if (s.substr(i, 3) == "abs")		babka.content = 'm';	//	abs
		else if (s.substr(i, 3) == "Ang")		babka.content = 'A';	//	Ang (-3.14159 : 3.14159]
		else if (s.substr(i, 3) == "ang")		babka.content = 'n';	//	ang [0 : 6.28319)
		else if (s.substr(i, 3) == "inf") {							//	inf
			babka.value.R = $inf;
			i = i + 2;
			obj.push_back(babka);
			continue;
		}
		else if (s.substr(i, 3) == "pl[")		babka.content = 'p';	//	pl[]

		else if (s.substr(i, 2) == "ln")		babka.content = 'l';	//	ln
		else if (s.substr(i, 2) == "tg") {		//	tg (tan)
			babka.content = 'T';
			i++;
			obj.push_back(babka);
			continue;
		}
		else if (s.substr(i, 2) == "pi") {
			i++;
			babka.type = 0;
			babka.value.R = pi;
			obj.push_back(babka);
			continue;
		}
		else if (s.substr(i, 2) == "Re")		babka.content = 'R';
		else if (s.substr(i, 2) == "Im")		babka.content = 'I';
		else if (s.substr(i, 2) == "f[")		babka.content = 'f';	//	f[]
		switch (babka.content) {								//	сдвиг символа
		case('1'): {
			i = i + 8;
			obj.push_back(babka);
			continue;
		}
		case('2'): {
			i = i + 7;
			obj.push_back(babka);
			continue;
		}
		case('c'):case('s'):case('t'):case('g'):case('d'):case('|'):case(':'): {
			i = i + 5;
			obj.push_back(babka);
			continue;
		}
		case('r'):case('='):case('!'): {
			i = i + 4;
			obj.push_back(babka);
			continue;
		}
		case('q'):case('['):case('5'):case('6'):case('z'):case('+'): {
			i = i + 3;
			obj.push_back(babka);
			continue;
		}
		case('S'):case('C'):case('e'):case('T'):case('G'):case('n'):case('m'):case('A'):case('D'):case('p'): {
			i = i + 2;
			obj.push_back(babka);
			continue;
		}
		case('l'):case('R'):case('I'):case('f'): {
			i++;
			obj.push_back(babka);
			continue;
		}
		}
		switch (s[i]) {
		case('.'):case(','): {
			dot = 1;
			break;
		}
		case('+'):case('-'):case('/'):case('^'):case('!'):case('%'): {
			babka.type = 1;
			babka.content = s[i];
			obj.push_back(babka);
			break;
		}
		case(')'):case(']'): {
			babka.content = s[i];
			obj.push_back(babka);
			break;
		}
		case('('): {
			if (!obj.empty()) if (obj.back().content == ')') {
				object one(0, (double)1);
				obj.push_back(one);
			}
			babka.content = s[i];
			obj.push_back(babka);
			break;
		}
		case('x'): {
			babka.type = 3;
			babka.content = 'x';
			obj.push_back(babka);
			if (type == 0) type = 1;
			break;
		}
		case('y'): {
			babka.type = 3;
			babka.content = 'y';
			obj.push_back(babka);
			if (type < 3) type = 2;
			break;
		}
		case('z'): {
			babka.type = 3;
			babka.content = 'z';
			obj.push_back(babka);
			if (type < 3) type = 2;
			break;
		}
		case('C'): {
			babka.type = 4;
			babka.content = 'C';
			obj.push_back(babka);
			type = 3;
			break;
		}
		case('e'): {
			babka.type = 0;
			babka.value.R = e;
			obj.push_back(babka);
			break;
		}
		case('i'): {
			babka.type = 0;
			babka.value.i = 1;
			obj.push_back(babka);
			break;
		}
		case('π'): {
			babka.type = 0;
			babka.value.R = pi;
			obj.push_back(babka);
			break;
		}
		case('='): {
			babka.type = 2;
			babka.content = ')';
			obj.push_back(babka);
			equal_index = obj.size();
			babka.type = 2;
			babka.content = '(';
			obj.push_back(babka);
			if (type < 3) type = 2;
			break;
		}
		}
	}
	if (debug) {
		cout << "\ninput\n";
		for (int i = 0; i < obj.size(); i++) {
			if (obj[i].type == 0) {
				cout << i << "   ";
				print_c(obj[i].value);
				cout << endl;
			}
			else cout << i << "   " << obj[i].content << endl;
		}
		cout << "eq index = " << equal_index << endl;
		char w;
		cin >> w;
	}
	NewGraph.equal_index = equal_index;
	if ((equal_index == 0) && (type == 2))	NewGraph.type = 4;
	else									NewGraph.type = type;
	NewGraph.fnc = obj;
	return NewGraph;
}

int bracket(vector<object> obj, int main, int i_max) { // возвращает номер закрывающей скобки для данной открывающей
	for (int i = main + 1; i <= i_max; i++) {
		if (i == obj.size()) return main;
		if (obj[i].content == '(')	i = bracket(obj, i, i_max) + 1;
		if (obj[i].content == ')')	return i;
	}
	return main;
}
int log_bracket(vector<object> obj, int main, int i_max) { // возвращает номер закрывающей скобки для данной открывающей
	for (int i = main + 1; i <= i_max; i++) {
		if (obj[i].content == '[')	i = log_bracket(obj, i, i_max) + 1;
		if (obj[i].content == ']')	return i;
	}
	return i_max + 1;
}
int first_0_priority(vector<object> obj, int i, int i_max) {	// возвращает номер перед +, - или )
	for (i; i < i_max; i++) {
		if (obj[i].content == '[')	i = 1 + log_bracket(obj, i + 1, i_max);
		if (obj[i].content == '(')	i = 1 + bracket(obj, i, i_max);
		if ((obj[i].content == '+') ||
			(obj[i].content == '-') ||
			(obj[i].content == ')') ||
			(obj[i].content == ']')
			)	return i - 1;
	}
	return i_max;
}
int first_1_priority(vector<object> obj, int i, int i_max) {	// возвращает номер перед +, -, * или /
	for (i; i < i_max; i++) {
		if (	(
					(obj[i].type == 0) ||
					(obj[i + 1].content == ')') ||
					(obj[i].type == 3)
				) &&
				(
					(obj[i + 1].content != '^') &&
					(obj[i + 1].content != '!')
				)
			)		return i;
		if (obj[i].content == '[')	i = 1 + log_bracket(obj, i + 1, i_max);
		if (obj[i].content == '(')	i = 1 + bracket(obj, i, i_max);
	}
	return i_max;
}

complex value(object obj, complex x, complex y) {
	if (obj.type == 3) {
		if (obj.content == 'a') return ans;
		if (obj.content == 'x') return x;
		if (obj.content == 'y') return y;
		if (obj.content == 'z') return x + i * y;
	}
	if (obj.type == 4) return Cnum;
	return obj.value;
}
complex calc(vector<object> obj, int first, int last, complex x, complex y) {
	if (debug) {
		cout << "\ncalc[" << first << ":" << last << "]\n";
		for (int i = 0; i < obj.size(); i++) {
			if (i == first) cout << "-------->\n";
			if (obj[i].type == 0) {
				cout << i << "   ";
				print_c(obj[i].value);
				cout << endl;
			}
			else cout << i << "   " << obj[i].content << endl;
			if (i == last) cout << "<--------\n";
		}
		cout << endl;
	}
	if (visual) {
		for (int i = first; i <= last; i++) {
			cout << " ";
			if (obj[i].type == 0)	print_c(obj[i].value);
			else					cout << obj[i].content;
		}
		cout << endl;
	}
	if (first < last) {
		int c;
		if ((obj[first].type == 0) || (obj[first].type > 2)) {
			if (obj[first + 1].type == 1) {
				switch (obj[first + 1].content) {
				case('!'): {
					obj[first + 1].value = fct(value(obj[first], x, y));
					obj[first + 1].type = 0;
					return calc(obj, first + 1, last, x, y);
				}
				case('^'): {
					c = first_1_priority(obj, first + 2, last);
					obj[c].value = power(value(obj[first], x, y), calc(obj, first + 2, c, x, y));
					obj[c].type = 0;
					return calc(obj, c, last, x, y);
				}
				case('/'): {
					c = first_1_priority(obj, first + 2, last);
					obj[c].value = value(obj[first], x, y) / calc(obj, first + 2, c, x, y);
					obj[c].type = 0;
					return calc(obj, c, last, x, y);
				}
				case('%'): {
					c = first_1_priority(obj, first + 2, last);
					obj[c].value = value(obj[first], x, y) % calc(obj, first + 2, c, x, y);
					obj[c].type = 0;
					return calc(obj, c, last, x, y);
				}
				case('+'): {
					c = first_0_priority(obj, first + 2, last);
					obj[c].value = value(obj[first], x, y) + calc(obj, first + 2, c, x, y);
					obj[c].type = 0;
					return calc(obj, c, last, x, y);
				}
				case('-'): {
					c = first_0_priority(obj, first + 2, last);
					obj[c].value = value(obj[first], x, y) - calc(obj, first + 2, c, x, y);
					obj[c].type = 0;
					return calc(obj, c, last, x, y);
				}
				}
			}
			c = first_1_priority(obj, first + 1, last);
			obj[c].value = value(obj[first], x, y) * calc(obj, first + 1, c, x, y);
			obj[c].type = 0;
			return calc(obj, c, last, x, y);
		}
		if (obj[first].content == '-') {
			c = first_1_priority(obj, first + 1, last);
			complex _res = calc(obj, first + 1, c, x, y);
			obj[c].value.R = -_res.R;
			obj[c].value.i = -_res.i;
			obj[c].type = 0;
			return calc(obj, c, last, x, y);
		}
		if (obj[first].content == '(') {
			c = bracket(obj, first, last);
			obj[c].value = calc(obj, first + 1, c - 1, x, y);
			obj[c].type = 0;
			return calc(obj, c, last, x, y);
		}
		if		(obj[first].content == '[') {
			int p = log_bracket(obj, first + 1, last);
			if ((obj[first + 1].type == 2) && (obj[first + 1].content != '('))	c = bracket(obj, p + 2, last);
			else																c = bracket(obj, p + 1, last);
			obj[c].value = ln(calc(obj, p + 1, c, x, y)) / ln(calc(obj, first + 1, p - 1, x, y));
			obj[c].type = 0;
			return calc(obj, c, last, x, y);
		}
		else if (obj[first].content == 'r') {
			int p = log_bracket(obj, first + 1, last);
			if ((obj[first + 1].type == 2) && (obj[first + 1].content != '('))	c = bracket(obj, p + 2, last);
			else																c = bracket(obj, p + 1, last);
			complex pow = calc(obj, first + 1, p - 1, x, y),
					floor = calc(obj, p + 1, c, x, y);
			if (pow.mod() == $inf)			obj[c].value = 1;
			else if (pow.mod() == 0)
				if (floor.mod() > 1)		obj[c].value = $inf;
				else if (floor.mod() == 1)	obj[c].value = 1;
				else						obj[c].value = 0;
			else							obj[c].value = power(floor, 1 / pow);
			obj[c].type = 0;
			return calc(obj, c, last, x, y);
		}
		else if (obj[first].content == 'f' || obj[first].content == 'p') {
			int p = log_bracket(obj, first + 1, last);
			if ((obj[first + 1].type == 2) && (obj[first + 1].content != '('))	c = bracket(obj, p + 2, last);
			else																c = bracket(obj, p + 1, last);
			complex index = calc(obj, first + 1, p - 1, x, y),
				arg = calc(obj, p + 1, c, x, y);
			if ((0 <= index.R) && (index.i == 0)) {
				if (obj[first].content == 'f' && index <  f.size()) obj[c].value = calc( f[(int)index.R].fnc, 0,  f[(int)index.R].fnc.size() - 1, arg.R, arg.i);
				if (obj[first].content == 'p' && index < pl.size())	obj[c].value = calc(pl[(int)index.R].fnc, 0, pl[(int)index.R].fnc.size() - 1, arg.R, arg.i);
			}
			else														obj[c].value == $NaN;
			obj[c].type = 0;
			return calc(obj, c, last, x, y);
		}
		else if (obj[first].content == ':') {
			char Operation = obj[first + 1].content, arcOperation;
			switch (Operation) {
			case('e'): {
				arcOperation = 'l';
				break;
			}
			case('1'): {
				arcOperation = 'l';
				break;
			}
			case('2'): {
				arcOperation = 'l';
				break;
			}
			case('l'): {
				arcOperation = 'e';
				break;
			}
			case('C'): {
				arcOperation = 'c';
				break;
			}
			case('c'): {
				arcOperation = 'C';
				break;
			}
			case('S'): {
				arcOperation = 's';
				break;
			}
			case('s'): {
				arcOperation = 'S';
				break;
			}
			case('T'): {
				arcOperation = 't';
				break;
			}
			case('t'): {
				arcOperation = 'T';
				break;
			}
			case('G'): {
				arcOperation = 'g';
				break;
			}
			case('g'): {
				arcOperation = 'G';
				break;
			}
			case('D'): {
				arcOperation = 'd';
				break;
			}
			case('d'): {
				arcOperation = 'D';
				break;
			}
			default: arcOperation = '\0';
			}
			complex res = x + i * y;
			vector<object> line;
			if (arcOperation != '\0') for (int n = 0; n < 16; n++) {
				object babka(2, Operation);
				line.push_back(babka);
				babka.content = '(';
				line.push_back(babka);
				babka.content = arcOperation;
				line.push_back(babka);
				babka.content = '(';
				line.push_back(babka);
				babka.type = 0;
				babka.content = '\0';
				babka.value = res;
				line.push_back(babka);
				babka.type = 2;
				babka.content = ')';
				line.push_back(babka);
				line.push_back(babka);
				res = calc(line, 0, line.size() - 1, x, y);
				line.clear();
			}
			obj[first + 1].value = res;
			obj[first + 1].type = 0;
			return calc(obj, first + 1, last, x, y);
		}
		c = bracket(obj, first + 1, last);
		complex res = calc(obj, first + 1, c, x, y);
		switch (obj[first].content) {
		case('m'): {
			obj[c].value.R = res.mod();
			obj[c].value.i = 0;
			break;
		}
		case('A'): {
			obj[c].value.R = arccos(res.R / res.mod());
			if (res.i < 0) obj[c].value.R = -obj[c].value.R;
			obj[c].value.i = 0;
			break;
		}
		case('n'): {
			obj[c].value.R = arccos(res.R / res.mod());
			if (res.i < 0) obj[c].value.R = 2 * pi - obj[c].value.R;
			obj[c].value.i = 0;
			break;
		}

		case('C'): {
			obj[c].value = cos(res);
			break;
		}
		case('c'): {
			obj[c].value = arccos(res);
			break;
		}
		case('S'): {
			obj[c].value = sin(res);
			break;
		}
		case('s'): {
			obj[c].value = pi / 2 - arccos(res);
			break;
		}
		case('T'): {
			obj[c].value = sin(res) / cos(res);
			break;
		}
		case('t'): {
			obj[c].value = D(res.R) * arccos(1 / power(1 + res * res, 0.5));
			break;
		}
		case('G'): {
			obj[c].value = cos(res) / sin(res);
			break;
		}
		case('g'): {
			obj[c].value = arccos(res / power(1 + res * res, 0.5));
			break;
		}
		case('5'): {
			obj[c].value = cos(res * i); // (exp(res) + exp(0 - res)) / 2;
			break;
		}
		case('6'): {
			obj[c].value = sin(res * i) / i;// (exp(res) - exp(0 - res)) / 2;
			break;
		}

		case('R'): {
			obj[c].value.R = res.R;
			obj[c].value.i = 0;
			break;
		}
		case('I'): {
			obj[c].value.R = res.i;
			obj[c].value.i = 0;
			break;
		}

		case('l'): {
			obj[c].value = ln(res);
			break;
		}
		case('e'): {
			obj[c].value = exp(res);
			break;
		}
		case('1'): {
			obj[c].value = exp(res, 1);
			break;
		}
		case('2'): {
			obj[c].value = exp(res, 0);
			break;
		}
		case('q'): {
			obj[c].value = power(res, 0.5);
			break;
		}

		case('D'): {
			obj[c].value = cycloid(res);
			break;
		}
		case('d'): {
			obj[c].value = arccyc(res);
			break;
		}

		case('='): {
			if (res.mod() != $inf && res.mod() >= 0)  obj[c].value = 1;
			else					obj[c].value = 0;
			break;
		}
		case('|'): {
			if (res.mod() < $inf)	obj[c].value = 1 / (1 + res.mod());
			else					obj[c].value = 0;
			break;
		}
		case('+'): {
			double a = fmod(res.R, 1), b = fmod(res.i, 1);
			if (a < 0) a = -a;
			if (b < 0) b = -b;
			if (a > 0.5) a = 1 - a;
			if (b > 0.5) b = 1 - b;
			if (a < b)	obj[c].value = a;
			else		obj[c].value = b;
			break;
		}

		case('z'): {
			obj[c].value = zeta(res);
			break;
		}
		case('!'): {
			obj[c].value = gamma(res);
			break;
		}
		}
		obj[c].type = 0;
		return calc(obj, c, last, x, y);
		
	}
	return value(obj[first], x, y);
}

void res_4(complex res) {
	double X = (res.R - x_min) / x_resolution, Y = (res.i - y_min) / y_resolution;
	complex s((int)X * x_resolution + x_min, (int)Y * y_resolution + y_min);
	double D = (res - s).mod();
	int i = X, j = Y;
	if (hyperbolic) {
		if (i >= 0) {
			i = (x_size + 1) * (1 - 1 / (2 * (double)i + 2));
		}
		else {
			i = (x_size + 1) / (2 - 2 * (double)i);
		}
		if (j >= 0) {
			j = (y_size + 1) * (1 - 1 / (2 * (double)j + 2));
		}
		else {
			j = (y_size + 1) / (2 - 2 * (double)j);
		}
	}
	int pixel = 0;
	if (D <= res8)				pixel++;
	if (D <= res4)				pixel++;
	if (D <= res2)				pixel++;
	if (D <= mid_resolution)	pixel++;
	if (D <= res05)				pixel++;
	if (D <= res025)			pixel++;
	if (D <= 0)					pixel++;
	pen(i, j, pixel);
}
void res(graph f) {
	vector<object> obj = f.fnc;
	switch (f.type) {
	case(1): {
		complex x, x05, y0;
		double m = 0;
		for (int i = 0; i <= x_size; i++) {
			if (hyperbolic) {
				if (i >= x_size / 2) {
					x.R = 1 / (2 - 2 * (double)i / x_size) - 1;
				}
				else {
					x.R = 1 - 1 / (2 * (double)i / x_size);
				}
			}
			else x.R = x_min + (double)i * x_resolution;
			x05.R = x_min + ((double)i - 0.5) * x_resolution;
			obj = f.fnc;
			complex function = calc(obj, 0, obj.size() - 1, x, 0);
			obj = f.fnc;
			complex y05 = calc(obj, 0, obj.size() - 1, x05, 0);
			if ((-0.001 < function.i) && (function.i < 0.001)) function.i = 0;
			if ((-0.001 < y05.i) && (y05.i < 0.001)) y05.i = 0;
			if (function.i == 0) {
				for (int y = 0; y <= y_size; y++) {
					if (hyperbolic) {
						if (y >= y_size / 2) {
							m = 1 / (2 - 2 * (double)y / y_size) - 1;
							if (((1 / (2 - 2 * ((double)y - 0.5) / y_size) - 1) <= function.R) && (function.R <= (1 / (2 - 2 * ((double)y - 0.166667) / y_size) - 1))) {
								matrix[i][y] = -5; // ,
								continue;
							}
							else if (((1 / (2 - 2 * ((double)y - 0.166667) / y_size) - 1) < function.R) && (function.R < (1 / (2 - 2 * ((double)y + 0.166667) / y_size) - 1))) {
								matrix[i][y] = -4; // =
								continue;
							}
							else if (((1 / (2 - 2 * ((double)y + 0.166667) / y_size) - 1) <= function.R) && (function.R < (1 / (2 - 2 * ((double)y + 0.5) / y_size) - 1))) {
								matrix[i][y] = -3; // "
								continue;
							}
						}
						else {
							m = 1 - 1 / (2 * (double)y / y_size);
							if (((1 - 1 / (2 * ((double)y - 0.5) / y_size)) <= function.R) && (function.R <= (1 - 1 / (2 * ((double)y - 0.166667) / y_size)))) {
								matrix[i][y] = -5;
								continue;
							}
							else if (((1 - 1 / (2 * ((double)y - 0.166667) / y_size)) < function.R) && (function.R < (1 - 1 / (2 * ((double)y + 0.166667) / y_size)))) {
								matrix[i][y] = -4;
								continue;
							}
							else if (((1 - 1 / (2 * ((double)y + 0.166667) / y_size)) <= function.R) && (function.R < (1 - 1 / (2 * ((double)y + 0.5) / y_size)))) {
								matrix[i][y] = -3;
								continue;
							}
						}
					}
					else {
						m = y_min + (double)y * y_resolution;
						if (((m - 0.5 * y_resolution) <= function.R) && (function.R <= (m - 0.166667 * y_resolution))) {
							matrix[i][y] = -5;
							continue;
						}
						else if (((m - 0.166667 * y_resolution) < function.R) && (function.R < (m + 0.166667 * y_resolution))) {
							matrix[i][y] = -4;
							continue;
						}
						else if (((m + 0.166667 * y_resolution) <= function.R) && (function.R < (m + 0.5 * y_resolution))) {
							matrix[i][y] = -3;
							continue;
						}
					}
					if ((y0.i == 0) && (y05.i == 0) && (i > 0))
					{
						if (((y0.R < y05.R) && (y05.R < m) && (m < function.R)) ||
							((y0.R > y05.R) && (y05.R > m) && (m > function.R)))
							matrix[i][y] = -6;
						if (((y0.R < m) && (m < y05.R) && (y05.R < function.R)) ||
							((y0.R > m) && (m > y05.R) && (y05.R > function.R)))
							matrix[i - 1][y] = -6;
					}
				}
			}
			y0 = function;
		}
		break;
	}
	case(2): {
		complex x, y;
		cout << "render   0%";
		double progress = 0;
		int percent = 0;
		for (int i = 0; i <= x_size; i++) {
			if (hyperbolic) {
				if (i >= x_size / 2) {
					x.R = 1 / (2 - 2 * (double)i / (x_size + 1)) - 1;
				}
				else {
					x.R = 1 - 1 / (2 * (double)i / (x_size + 1));
				}
			}
			else x.R = x_min + (double)i * x_resolution;
			for (int j = 0; j <= y_size; j++) {
				if (hyperbolic) {
					if (j >= y_size / 2) {
						y.R = 1 / (2 - 2 * (double)j / (y_size + 1)) - 1;
					}
					else {
						y.R = 1 - 1 / (2 * (double)j / (y_size + 1));
					}
				}
				else y.R = y_min + (double)j * y_resolution;
				obj = f.fnc;
				double D = (calc(obj, 0, f.equal_index - 1, x, y) - calc(obj, f.equal_index, obj.size() - 1, x, y)).mod();
				int pixel = 0;
				if (D <= res8)				pixel++;
				if (D <= res4)				pixel++;
				if (D <= res2)				pixel++;
				if (D <= mid_resolution)	pixel++;
				if (D <= res05)				pixel++;
				if (D <= res025)			pixel++;
				if (D <= 0)					pixel++;
				pen(i, j, pixel);
				progress++;
				percent = 100 * progress / (x_size + 1) / (y_size + 1);
				cout << "\b\b";
				if (percent > 9) cout << '\b';
				if (percent > 99) cout << '\b';
				cout << percent << "%";
			}
		}
		cout << endl;
		break;
	}
	case(3): {
		cout << "render started...";
		complex x, y;
		double mid_resolution = exp(ln(x_resolution * y_resolution) / 2);
		for (int i = 0; i <= x_size; i++) {
			x.R = x_min + (double)i * x_resolution;
			for (int j = 0; j <= y_size; j++) {
				y.R = y_min + (double)j * y_resolution;
				double mod1, mod2;
				Cnum = 0;
				for (int k = 0; k < 10; k++) {
					obj = f.fnc;
					Cnum = calc(obj, 0, obj.size() - 1, x, y);
				}
				mod1 = -Cnum.mod();
				obj = f.fnc;
				Cnum = calc(obj, 0, obj.size() - 1, x, y);
				mod1 += Cnum.mod();
				mod2 = -Cnum.mod();
				obj = f.fnc;
				Cnum = calc(obj, 0, obj.size() - 1, x, y);
				mod2 += Cnum.mod();
				if (mod1 < 0) mod1 = -mod1;
				if (mod2 < 0) mod2 = -mod2;
				mod1 = mod2 / mod1;
				int pixel = 0;
				if (mod1 <= res823543)		pixel++;
				if (mod1 <= res46656)		pixel++;
				if (mod1 <= res3125)		pixel++;
				if (mod1 <= res256)			pixel++;
				if (mod1 <= res27)			pixel++;
				if (mod1 <= res4)			pixel++;
				if (mod1 <= mid_resolution)	pixel++;
				pen(i, j, pixel);
			}
		}
		break;
	}
	case(4): {
		complex x, y;
		for (int i = -500; i <= 500; i++) {
			cout << '*';
			x.R = (double)i / 50;
			if (i % 50 == 0) {
				for (int j = -500; j <= 500; j++) {
					y.R = (double)j / 50;
					complex res = calc(obj, 0, obj.size() - 1, x, y);
					if (hyperbolic) res_4(res);
					else if ((res.R <= x_max) && (x_min <= res.R) && (res.i <= y_max) && (y_min <= res.i)) res_4(res);
				}
			}
			else {
				for (int j = -10; j <= 10; j++) {
					y.R = (double)j;
					complex res = calc(obj, 0, obj.size() - 1, x, y);
					if (hyperbolic) res_4(res);
					else if ((res.R <= x_max) && (x_min <= res.R) && (res.i <= y_max) && (y_min <= res.i)) res_4(res);
				}
			}
		}
		break;
	}
	}
}
float res(graph f, double x, double y) {
	vector<object> obj = f.fnc;
	switch (f.type) {
	case(2): {
		float res = (calc(obj, 0, f.equal_index - 1, x, y) - calc(obj, f.equal_index, obj.size() - 1, x, y)).mod() + 1;
		res = 1 / (res * res);
		return res;
	}
	}
}

int main(){
	string s;
	ifstream help_file("../help.txt");
	getline(help_file, inst, '\0');
	clear_matrix();
	cout << "	[";
	for (int i = 0; i <= x_size; i++) cout << "_";
	cout << "]\n";
	cout << "type \"help\" to view help page\n";
	while (1) {
		cout << "\n   ";
		getline(cin, s, '\n');
		if (s[0] == '\n') s = s.substr(1, s.length() - 1);
		cout << endl;
		if (s == "" || command(s)) continue;
		graph NewGraph = parse(s);
		switch (NewGraph.type) {
		case(0): {
			complex result = calc(NewGraph.fnc, 0, NewGraph.fnc.size() - 1, 0, 0);
			ans = result;
			if (rounded) {
				if ((-0.0001 < result.R) && (result.R < 0.0001)) result.R = 0;
				if ((-0.0001 < result.i) && (result.i < 0.0001)) result.i = 0;
			}
			cout << "	" << result.toString() << "\n\n";
			break;
		}
		case(1): {
			f.push_back(NewGraph);
			draw();
			break;
		}
		case(4): {
			pl.push_back(NewGraph);
			draw();
			break;
		}
		default: {
			eq.push_back(NewGraph);
			draw();
		}
		}
	}
	return 0;
}
bool command(string command) {
	if (command == "clear") {
		f.clear();
		eq.clear();
		pl.clear();
		system("cls");
		if (extends) {
			cout << "	debug      = " << debug << '\n';
			cout << "	round      = " << rounded << '\n';
			cout << "	axis       = " << axis << '\n';
			cout << "	visual     = " << visual << '\n';
			cout << "	hyperbolic = " << hyperbolic << '\n';
			cout << "	render     = " << rounded << '\n';
			cout << "	extends    = " << rounded << '\n';
			cout << "	answer     = " << ans.toString() << '\n';
		}
		clear_matrix();
		draw();
		return 1;
	}
	if (command == "erase") {
		system("cls");
		if (extends) {
			cout << "	debug      = " << debug << '\n';
			cout << "	round      = " << rounded << '\n';
			cout << "	axis       = " << axis << '\n';
			cout << "	visual     = " << visual << '\n';
			cout << "	hyperbolic = " << hyperbolic << '\n';
			cout << "	render     = " << rounded << '\n';
			cout << "	extends    = " << rounded << '\n';
			cout << "	answer     = " << ans.toString() << '\n';
		}
		return 1;
	}
	if (command == "del f") {
		if (f.empty()) return 1;
		int j;
		cout << "enter graph number: ";
		cin >> j;
		f.erase(f.begin() + j);
		clear_matrix();
		system("cls");
		draw();
		return 1;
	}
	if (command == "del eq") {
		if (eq.empty()) return 1;
		int j;
		cout << "enter equality number: ";
		cin >> j;
		eq.erase(eq.begin() + j);
		clear_matrix();
		system("cls");
		draw();
		return 1;
	}
	if (command == "del pl") {
		if (pl.empty()) return 1;
		int j;
		cout << "enter plane number: ";
		cin >> j;
		pl.erase(pl.begin() + j);
		clear_matrix();
		system("cls");
		draw();
		return 1;
	}
	
	if (command == "scale") {
		cout << "enter x_min and x_max: ";
		cin >> x_min >> x_max;
		cout << "enter y_min and y_max: ";
		cin >> y_min >> y_max;
		x_resolution = (x_max - x_min) / x_size, y_resolution = (y_max - y_min) / y_size;
		if (x_resolution < 0) x_resolution = -x_resolution;
		if (y_resolution < 0) y_resolution = -y_resolution;
		mid_resolution = exp(ln(x_resolution * y_resolution) / 2);

		res025 = 0.25 * mid_resolution;
		res05 = 0.5 * mid_resolution;
		res2 = 2 * mid_resolution;
		res4 = 4 * mid_resolution;
		res8 = 8 * mid_resolution;

		res27 = 27 * mid_resolution;
		res256 = 256 * mid_resolution;
		res3125 = 3125 * mid_resolution;
		res46656 = 46656 * mid_resolution;
		res823543 = 823543 * mid_resolution;

		clear_matrix();
		draw();
		return 1;
	}
	if (command == "resize") {
		cout << "x_size & y_size: ";
		cin >> x_size >> y_size;
		if (x_size < 1)		x_size = 1;
		if (x_size > 200)	x_size = 200;
		if (y_size < 1)		y_size = 1;
		if (y_size > 100)	y_size = 100;
		x_resolution = (x_max - x_min) / x_size, y_resolution = (y_max - y_min) / y_size;
		if (x_resolution < 0) x_resolution = -x_resolution;
		if (y_resolution < 0) y_resolution = -y_resolution;
		mid_resolution = exp(ln(x_resolution * y_resolution) / 2);

		res025 = 0.25 * mid_resolution;
		res05 = 0.5 * mid_resolution;
		res2 = 2 * mid_resolution;
		res4 = 4 * mid_resolution;
		res8 = 8 * mid_resolution;

		res27 = 27 * mid_resolution;
		res256 = 256 * mid_resolution;
		res3125 = 3125 * mid_resolution;
		res46656 = 46656 * mid_resolution;
		res823543 = 823543 * mid_resolution;

		clear_matrix();
		draw();
		return 1;
	}
	if (command == "show") {
		int j;
		if (!f.empty()) {
			if (!eq.empty()) {
				int w;
				cout << "graph \"0\" or equality \"1\": ";
				cin >> w;
				if (w) {
				show_eq:
					cout << "enter equality number: ";
					cin >> j;
					eq[j].show = true;
				}
				else {
				show_f:
					cout << "enter graph number: ";
					cin >> j;
					f[j].show = true;
				}
			}
			else goto show_f;
		}
		else if (!eq.empty()) goto show_eq;
		clear_matrix();
		draw();
		return 1;
	}
	if (command == "hide") {
		int j;
		if (!f.empty()) {
			if (!eq.empty()) {
				int w;
				cout << "graph \"0\" or equality \"1\": ";
				cin >> w;
				if (w) {
				hide_eq:
					cout << "enter equality number: ";
					cin >> j;
					eq[j].show = false;
				}
				else {
				hide_f:
					cout << "enter graph number: ";
					cin >> j;
					f[j].show = false;
				}
			}
			else goto hide_f;
		}
		else if (!eq.empty()) goto hide_eq;
		clear_matrix();
		draw();
		return 1;
	}
	if (command == "axis") {
		axis = !axis;
		if (axis) cout << "	ON\n";
		else	  cout << "	OFF\n";
		clear_matrix();
		draw();
		return 1;
	}
	if (command == "grid") {
		grid = !grid;
		if (grid) cout << "	ON\n";
		else	  cout << "	OFF\n";
		return 1;
	}
	if (command == "visual") {
		visual = !visual;
		if (visual) cout << "	ON\n";
		else			cout << "	OFF\n";
		return 1;
	}
	if (command == "render") {
		render = !render;
		if (render) cout << "	ON\n";
		else		cout << "	OFF\n";
		return 1;
	}
	if (command == "hyper") {
		hyperbolic = !hyperbolic;
		if (hyperbolic) cout << "	ON\n";
		else			cout << "	OFF\n";
		clear_matrix();
		draw();
		return 1;
	}

	if (command == "round") {
		rounded = !rounded;
		if (rounded)	cout << "	ON\n";
		else			cout << "	OFF\n";
		return 1;
	}
	if (command == "optimized") {
		optimized = !optimized;
		if (optimized)	cout << "	ON\n";
		else			cout << "	OFF\n";
		return 1;
	}
	if (command == "iterations") {
		cout << "	" << iterations << " ==[0 : 256]=> ";
		cin >> iterations;
		if (iterations < 0) iterations = 0;
		else if (iterations > 256) iterations = 256;
		return 1;
	}

	if (command == "help") {
		cout << inst;
		return 1;
	}
	if (command == "ver") {
		cout << ver;
		return 1;
	}
	if (command == "extends") {
		extends = !extends;
		if (extends) {
			cout << "	ON\n";
			if (extends) {
				cout << "	debug      = " << debug << '\n';
				cout << "	round      = " << rounded << '\n';
				cout << "	axis       = " << axis << '\n';
				cout << "	grid       = " << grid << '\n';
				cout << "	visual     = " << visual << '\n';
				cout << "	hyperbolic = " << hyperbolic << '\n';
				cout << "	render     = " << rounded << '\n';
				cout << "	extends    = " << rounded << '\n';
				cout << "	answer     = " << ans.toString() << '\n';
			}
		}
		else			cout << "	OFF\n";
		return 1;
	}

	if (command == "print") {
		// img[(heigth - y - 1) * width + x]
		int width, heigth;
		cout << "width[x] heigth[y]\n";
		cin >> width >> heigth;
		heigth++;
		width++;
		double scale_img = exp(ln((double)width / (y_max - y_min) * (double)heigth / (x_max - x_min)) / 2);
		float* imgR		= (float*)calloc(width * heigth, sizeof(float));
		float* imgG		= (float*)calloc(width * heigth, sizeof(float));
		float* imgB		= (float*)calloc(width * heigth, sizeof(float));
		float* imgEmpty = (float*)calloc(width * heigth, sizeof(float));
		int x_slice = x_min * scale_img;
		int y_slice = y_min * scale_img;
		if (grid) {
			float delta = fmod(y_min, 1);
			for (int y = 1; y <= (y_max - y_min); y++) {
				for (int x = 0; x < width; x++) {
					int cord = (double)((y + delta) * heigth) / (y_max - y_min) + 0.5;
					imgR = pen(imgR, width, heigth, x, cord, 0.125);
					imgG = pen(imgG, width, heigth, x, cord, 0.125);
					imgB = pen(imgB, width, heigth, x, cord, 0.125);
				}
			}
			delta = fmod(x_min, 1);
			for (int x = 1; x <= (x_max - x_min); x++) {
				for (int y = 0; y < heigth; y++) {
					int cord = (double)((x + delta) * width) / (x_max - x_min) + 0.5;
					imgR = pen(imgR, width, heigth, cord, y, 0.125);
					imgG = pen(imgG, width, heigth, cord, y, 0.125);
					imgB = pen(imgB, width, heigth, cord, y, 0.125);
				}
			}
		}
		if (axis) {
			if ((-width < x_slice) && (x_slice <= 0))
				for (int y = 0; y < heigth; y++) {
					imgR = pen(imgR, width, heigth, 1 - x_slice, y, 0.25);
					imgG = pen(imgG, width, heigth, 1 - x_slice, y, 0.25);
					imgB = pen(imgB, width, heigth, 1 - x_slice, y, 0.25);
				}
					
			if ((-heigth < y_slice) && (y_slice <= 0))
				for (int x = 0; x < width; x++) {
					imgR = pen(imgR, width, heigth, x, heigth + y_slice - 1, 0.25);
					imgG = pen(imgG, width, heigth, x, heigth + y_slice - 1, 0.25);
					imgB = pen(imgB, width, heigth, x, heigth + y_slice - 1, 0.25);
				}
		}
		for (int k = 0; k < f.size(); k++) {
			complex res, last_res = calc(f[k].fnc, 0, f[k].fnc.size() - 1, x_min - 1 / scale_img, 0);
			int y, last_y = heigth - (last_res.R - y_min) * scale_img;
			const float red_intensity = 0.625 + cos((double)k * 1.5) * 0.375,
						green_intensity = 0.625 + cos((double)k * 1.5 + 2.0944) * 0.375,
						blue_intensity = 0.625 + cos((double)k * 1.5 - 2.0944) * 0.375;
			for (int x = 0; x < width; x++) {
				res = calc(f[k].fnc, 0, f[k].fnc.size() - 1, x_min + x / scale_img, 0),
				y = heigth - (res.R - y_min) * scale_img;
				int min = heigth, max = 0;
				if (     y < min) min = y;
				if (last_y < min) min = last_y;
				if (     y > max) max = y;
				if (last_y > max) max = last_y;
				if (res.i == 0 && ((0 <= max && max < heigth) || (0 <= min && min < heigth))) {
					if (min == max) max++;
					if (max >= heigth) max = heigth - 1;
					if (min < 0) min = 0;
					for (int j = min; j < max; j++) {
						if ((0 <= j) && (j < heigth)) {
							imgR = pen(imgR, width, heigth, x, j, red_intensity);
							imgG = pen(imgG, width, heigth, x, j, green_intensity);
							imgB = pen(imgB, width, heigth, x, j, blue_intensity);
						}
					}
				}
				last_res = res,
				last_y = y;
			}
		}
		for (int i = 0; i < eq.size(); i++) {
			if (eq[i].type == 2) {
				cout << "render   0%";
				double progress = 0;
				int percent = 0;
				const float red_intensity   = 1.625 - cos((double)i * 1.5 + 2.0944) * 0.375,
							green_intensity = 1.625 - cos((double)i * 1.5 - 2.0944) * 0.375,
							blue_intensity  = 1.625 - cos((double)i * 1.5) * 0.375;
				for (int x = 0; x < width; x++)
					for (int y = 0; y < heigth; y++) {
						float w = res(eq[i], x / scale_img + x_min, y / scale_img + y_min);
						imgR = penAdd(imgR, width, heigth, x, y, pow(w, red_intensity));
						imgG = penAdd(imgG, width, heigth, x, y, pow(w, green_intensity));
						imgB = penAdd(imgB, width, heigth, x, y, pow(w, blue_intensity));
						progress++;
						percent = 100 * progress / heigth / width;
						cout << "\b\b";
						if (percent > 9) cout << '\b';
						if (percent > 99) cout << '\b';
						cout << percent << '%';
					}
				cout << "\n";
			}
		}
		for (int h = 0; h < pl.size(); h++) {
			cout << "render   0%";
			double progress = 0;
			int percent = 0;
			for (int x = 0; x < width; x++)
				for (int y = 0; y < heigth; y++) {
					complex res = calc(pl[h].fnc, 0, pl[h].fnc.size() - 1, x / scale_img + x_min, y / scale_img + y_min);
					double	angle = arccos(res.R / res.mod()),
						absolute = 1 - 1 / (res.mod() + 1);
					if (res.i < 0) angle = -angle;
					imgR = pen(imgR, width, heigth, x, y, (0.5 + res.R / res.mod() / 2) * absolute);
					imgG = pen(imgG, width, heigth, x, y, (0.5 + cos(angle + 2 * pi / 3) / 2) * absolute);
					imgB = pen(imgB, width, heigth, x, y, (0.5 + cos(angle - 2 * pi / 3) / 2) * absolute);
					progress++;
					percent = 100 * progress / heigth / width;
					cout << "\b\b";
					if (percent > 9) cout << '\b';
					if (percent > 99) cout << '\b';
					cout << percent << '%';
				}
			cout << "\n";
		}
		BMPWriter::write_image(imgR,     imgG,     imgB, heigth, width, (char*)"../bmp images/graphRGB.bmp");
		BMPWriter::write_image(imgR, imgEmpty, imgEmpty, heigth, width, (char*)"../bmp images/graphR.bmp");
		BMPWriter::write_image(imgEmpty, imgG, imgEmpty, heigth, width, (char*)"../bmp images/graphG.bmp");
		BMPWriter::write_image(imgEmpty, imgEmpty, imgB, heigth, width, (char*)"../bmp images/graphB.bmp");
		cout << "\nSuccess!\n";
		return 1;
	}
	return 0;
}
void clear_matrix() {
	for (int i = 0; i <= x_size; i++) {
		for (int j = 0; j <= y_size; j++) {
			matrix[i][j] = 0;
			if (hyperbolic) {
				if (i == x_size / 2)		matrix[i][j] = -2;		//	Oy
				else if (j == y_size / 2)	matrix[i][j] = -1;		//	Ox
			}
			else if (axis) {
				if (((-0.5 * x_resolution) <= (x_min + (double)i * x_resolution)) &&
					((x_min + (double)i * x_resolution) <= (0.5 * x_resolution)))			matrix[i][j] = -2;		//	Oy
				else if (((-0.5 * y_resolution) <= (y_min + (double)j * y_resolution)) &&
					((y_min + (double)j * y_resolution) <= (0.5 * y_resolution)))			matrix[i][j] = -1;		//	Ox
			}
		}
	}
}
void draw() {
	if ((eq.empty()) && (f.empty()) && (pl.empty())) {
		cout << "	[";
		for (int i = 0; i <= x_size; i++) cout << "_";
		cout << "]\n";
		return;
	}
	if (render) {
		bool v = 0;
		if (visual) {
			v = 1;
			visual = 0;
		}
		optimized = 1;
		for (int i = 0; i < pl.size(); i++) if (pl[i].show) res(pl[i]);
		for (int i = 0; i < eq.size(); i++) if (eq[i].show) res(eq[i]);
		optimized = 0;
		for (int i = 0; i < f.size(); i++) if (f[i].show) res(f[i]);
		if (v) visual = 1;
	}
	system("cls");
	for (int i = 0; i < pl.size(); i++) {
		cout << "	plane[" << i << "] : " << pl[i].name;
		if (!pl[i].show) cout << "   (hidden)";
		cout << endl;
	}
	for (int i = 0; i < eq.size(); i++) {
		cout << "	eq[" << i << "] : " << eq[i].name;
		if (!eq[i].show) cout << "   (hidden)";
		cout << endl;
	}
	for (int i = 0; i < f.size(); i++) {
		cout << "	f[" << i << "](x) = " << f[i].name;
		if (!f[i].show) cout << "   (hidden)";
		cout << endl;
	}
	cout << "	.";
	for (int i = 0; i <= x_size; i++) cout << "-";
	cout << ".\n";
	if (render) for (int y = y_size; y >= 0; y--) {
		cout << "	|";
		for (int x = 0; x <= x_size; x++)
			switch (matrix[x][y]) {
			case(-6):{
				cout << ':';
				break;
			}
			case(-5): {
				cout << ',';
				break;
			}
			case(-4): {
				cout << '=';
				break;
			}
			case(-3): {
				cout << '"';
				break;
			}
			case(-2): {
				cout << '|';
				break;
			}
			case(-1): {
				cout << '~';
				break;
			}
			case(0): {
				cout << ' ';
				break;
			}
			case(1): {
				cout << '-';
				break;
			}
			case(2): {
				cout << '~';
				break;
			}
			case(3): {
				cout << '=';
				break;
			}
			case(4): {
				cout << 'o';
				break;
			}
			case(5): {
				cout << 'O';
				break;
			}
			case(6): {
				cout << '#';
				break;
			}
			case(7): {
				cout << '0';
				break;
			}
			}
		cout << "|\n";
	}
	cout << "	'";
	for (int i = 0; i <= x_size; i++) cout << "-";
	cout << "'\n";
	if (hyperbolic) cout << "	[hyperbolic mode]\n";
	else {
		cout << "	x [" << x_min << " : " << x_max << "]\n";
		cout << "	y [" << y_min << " : " << y_max << "]\n";
	}
}