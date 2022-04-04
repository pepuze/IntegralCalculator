#include <stack>
#include <queue>
#include <iostream>
#include <string>
#include "functions.h"

int main() {

	while (1) {
		std::string func;
		std::cout << "Input function:\n";
		std::getline(std::cin, func);

		Function function(func);

		double a, b;
		std::cout << "Integral from a to b\na = ";
		std::cin >> a;
		std::cout << "b = ";
		std::cin >> b;
		std::cout << "Integrals:\n";
		std::cout << "Rectangle method: " << function.calcIntegral(a, b, QuadFormula::RectangleMethod, 10) << std::endl;
		std::cout << "Trapezoid method: " << function.calcIntegral(a, b, QuadFormula::TrapezoidMethod, 10) << std::endl;
		std::cout << "Simpson method: " << function.calcIntegral(a, b, QuadFormula::SimpsonMethod, 10) << std::endl << std::endl;
		std::cin.ignore();
	}
	return 0;
}