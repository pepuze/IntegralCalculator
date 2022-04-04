#include <stack>
#include <queue>
#include <string>
#include <cmath>
#include <iostream>
#include "functions.h"

/*

	functions.h - PARSER and FUNCTION CALCULATION

*/

/*

	PARSER~

*/

/*Tokens:
	operators: (+-/*^) and functions (sin cos tan cot ln lg)
	operands: constant numbers (5; 3.14; 4325.8457), variables (x)
*/

const std::string funcs = "sin|cos|tan|cot|ln|lg|sqrt", ops = "+-*/^";

bool isDigitOrDecimal(const char c) {
	return ((c >= '0' && c <= '9') || c == '.');
}

bool isLetter(const char c) {
	return (c >= 'a' && c <= 'z');
}

bool isRightAssociative(const std::string op) {
	if (op == "^") return 1;
	return 0;
}

bool checkFuncSyntax(std::string function) {
	return 1;
}

int getOperandPriority(const std::string op) {
	if (op == "+" || op == "-") return 1;
	if (op == "*" || op == "/") return 2;
	if (op == "^")				return 3;
	return 0;
}

TokenType getTokenType(const std::string value) {
	if (ops.find(value) != -1)		  return TokenType::Operator;
	if (funcs.find(value) != -1)      return TokenType::Function;
	if (value == ")" || value == "(") return TokenType::Parenthesis;
	return TokenType::Operand;
}

std::queue<Token> funcToPostfix(std::string function) {
	std::queue<Token> output;
	std::stack<Token> opStack;

	unsigned int i = 0;
	while (i < function.size()) {
		std::string value;

		//Reading token value
		if (function[i] == ' ') {
			++i;
			continue;
		}
		if (isDigitOrDecimal(function[i]) || function[i] == 'x') {
			while ((isDigitOrDecimal(function[i]) || function[i] == 'x') && i < function.size()) value += function[i++];
		}
		else if (isLetter(function[i])) {
			while (isLetter(function[i]) && i < function.size()) value += function[i++];
		}
		else value = function[i++];

		Token token(value);
		//What to do depending on the type of token
		switch (token.type) {

			//Function - put is on operator stack
		case TokenType::Function:
			opStack.push(token);
			break;

			//Operator - if there are operators on top of operator stack (not left parenthesis)
			//that have higher priority than current operator - pop them into output
			//
			//if not - put current operator on operator stack
		case TokenType::Operator:
			while (!opStack.empty() && opStack.top().value != "(") {
				Token topOperator = opStack.top();
				if (topOperator.priority > token.priority || (topOperator.priority == token.priority && !token.rightAssociative)) {
					output.push(topOperator);
					opStack.pop();
					continue;
				}
				break;
			}
			opStack.push(token);
			break;

			//Parenthesis - 
			//left parenthesis - put is on operator stack
			//right parenthesis - pop every operator in ouput until left parenthesis is found
			//then pop left parenthesis (if there is a function on top of the stack - pop it to the ouput) and discard token
		case TokenType::Parenthesis:
			if (token.value == ")") {
				while (opStack.top().value != "(") {
					output.push(opStack.top());
					opStack.pop();
				}
				opStack.pop();
				if (opStack.top().type == TokenType::Function) {
					output.push(opStack.top());
					opStack.pop();
				}
				break;
			}
			opStack.push(token);
			break;

			//Operand - put in in output
		case TokenType::Operand:
			output.push(token);
			break;
		}
	}

	//Pop all remainding operands in ouput
	while (!opStack.empty()) {
		output.push(opStack.top());
		opStack.pop();
	}

	return output;
}


/*

	~PARSER

*/

/*

	FUNCTION CALCULATION~

*/

double calcFunc(const double a, std::string function) {
	if (function == "sin")	return sin(a);
	if (function == "cos")	return cos(a);
	if (function == "tan")	return tan(a);
	if (function == "cot")	return 1.0 / tan(a);
	if (function == "lg")	return log10(a);
	if (function == "ln")	return log(a);
	if (function == "sqrt") return sqrt(a);
}

double calcOperator(const double a, const double b, const char op) {
	switch (op) {
	case '+':
		return a + b;
	case '-':
		return a - b;
	case '*':
		return a * b;
	case '/':
		return a / b;
	case '^':
		return pow(a, b);
	default:
		return -1;
	}
}

void Function::printPostfix() {
	std::queue<Token> funcCopy = function;
	while (!funcCopy.empty()) {
		std::cout << funcCopy.front().value << " ";
		funcCopy.pop();
	}
	return;
}

double Function::calc(const double x) {
	std::queue<Token> funcCopy = function;
	std::stack<double> stck;
	double a, b;

	while (!funcCopy.empty()) {
		Token token = funcCopy.front();
		switch (token.type) {

		case TokenType::Operand:
			if (token.value == "x") stck.push(x);
			else stck.push(std::stod(token.value));
			funcCopy.pop();
			break;

		case TokenType::Operator:
			b = stck.top(); stck.pop();
			a = stck.top(); stck.pop();
			stck.push(calcOperator(a, b, token.value[0]));
			funcCopy.pop();
			break;

		case TokenType::Function:
			a = stck.top(); stck.pop();
			stck.push(calcFunc(a, token.value));
			funcCopy.pop();
			break;
		}
	}

	return stck.top();
}


//Integral calculations:
//Rectangle method:
double Function::calcIntegralRectangleMethod(const double a, const double b, const unsigned int nSegments) {
	double result = 0;
	const double h = (b - a) / (nSegments * 1.0);

	if (nSegments < 2) result = calc((a + b) / 2);
	else {
		for (unsigned int i = 1; i <= nSegments; ++i) result += calc(a + (i * 2.0 - 1) * h / 2);
	}

	return result * h;
}

//Trapezoid method
double Function::calcIntegralTrapezoidMethod(const double a, const double b, const unsigned int nSegments) {
	const double h = (b - a) / (nSegments * 1.0);
	double result = calc(a) + calc(b);

	for (unsigned int i = 1; i < nSegments; ++i) result += 2 * calc(a + h * i);

	return result * (b - a) / (2.0 * nSegments);
}

//Simpson method - doesnt work correctly (?)
double Function::calcIntegralSimpsonMethod(const double a, const double b, unsigned int nSegments) {
	double result = calc(a) + calc(b);
	const double h = (b - a) / (nSegments * 1.0);
	nSegments *= 2;

	if (nSegments < 2) result += 4 * calc((a + b) / 2);
	else {
		for (unsigned int i = 2; i < nSegments; i += 2) result += 2 * calc(a + h * i);
		for (unsigned int i = 1; i < nSegments; i += 2) result += 4 * calc(a + h * i);
	}

	return result * (b - a) / (6.0 * nSegments);
}

double Function::calcIntegral(const double a, const double b, QuadFormula method = QuadFormula::RectangleMethod, const unsigned int nSegments = 1) {
	switch (method) {
	case QuadFormula::RectangleMethod:
		return calcIntegralRectangleMethod(a, b, nSegments);
	case QuadFormula::TrapezoidMethod:
		return calcIntegralTrapezoidMethod(a, b, nSegments);
	case QuadFormula::SimpsonMethod:
		return calcIntegralSimpsonMethod(a, b, nSegments);
	}
}

/*

	~FUNCTION CALCULATION

*/