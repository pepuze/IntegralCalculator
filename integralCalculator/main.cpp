#include <stack>
#include <queue>
#include <cmath>
#include <iostream>
#include <string>
#include <stdio.h>
#include "functions.h"
#include <SFML/Graphics.hpp>

const double dx = 0.001;

int main() {

	while (1) {
		std::string func, leftBoundStr, rightBoundStr, nSegmentsStr;
		std::cout << "Input function y(x):\n";
		std::getline(std::cin, func);

		for (char& ch : func) ch = tolower(ch);
		if (!checkFuncSyntax(func)) {
			continue;
		}

		Function function(func);
		double leftBound, rightBound;
		unsigned int nSegments = 20;
		std::cout << "Input integration limits [a,b]\na = ";
		std::cin >> leftBoundStr;

		try {
			leftBound = std::stod(leftBoundStr);
		}
		catch (...) {
			std::cout << "Invalid input\n\n";
			std::cin.ignore();
			continue;
		}

		std::cout << "b = ";
		std::cin >> rightBoundStr;

		try {
			rightBound = std::stod(rightBoundStr);
			if (rightBound < leftBound) throw(1);
		}
		catch (const int num) {
			std::cout << "Invalid bounds: a cannot be greater than b\n\n";
			std::cin.ignore();
			continue;
		}
		catch (...) {
			std::cout << "Invalid input\n\n";
			std::cin.ignore();
			continue;
		}

		std::cout << "Number of segments: ";
		std::cin >> nSegmentsStr;

		try {
			nSegments = std::stoul(nSegmentsStr);
			if (nSegments < 1) throw(1);
		}
		catch (const int num) {
			std::cout << "Number of segments cannot be 0 or less\n\n";
			std::cin.ignore();
			continue;
		}
		catch (...) {
			std::cout << "Invalid input\n\n";
			std::cin.ignore();
			continue;
		}

		std::cout << "Integrals:\n";
		std::cout << "Rectangle method: " << std::fixed << function.calcIntegral(leftBound, rightBound, QuadFormula::RectangleMethod, nSegments) << std::endl;
		std::cout << "Trapezoid method: " << std::fixed << function.calcIntegral(leftBound, rightBound, QuadFormula::TrapezoidMethod, nSegments) << std::endl;
		std::cout << "Simpson method: "   << std::fixed << function.calcIntegral(leftBound, rightBound, QuadFormula::SimpsonMethod, nSegments)   << std::endl << std::endl;
		
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		sf::RenderWindow window(sf::VideoMode(1024, 768), "Graph", sf::Style::Default, settings);
		sf::View view(sf::Vector2f(0, 0), sf::Vector2f(0, 0));
		view.setSize(1024, 768);

		const unsigned int nPoints = int(floor((rightBound - leftBound) / dx));
		const unsigned int nPointsMethods = nSegments * 4;

		sf::VertexArray graph(sf::LinesStrip, nPoints);					     bool toggleGraph	  = 1;
		sf::VertexArray graphRectangle(sf::LinesStrip, nPointsMethods);      bool toggleRectangle = 1;
		sf::VertexArray graphTrapezoid(sf::LinesStrip, nPointsMethods);      bool toggleTrapezoid = 1;
		sf::VertexArray graphSimpson(sf::LinesStrip, nPoints+nSegments*2+1); bool toggleSimpson   = 1;
		sf::VertexArray oX(sf::Lines, 2);
		sf::VertexArray oY(sf::Lines, 2);

		oX[0].color = oX[1].color = oY[0].color = oY[1].color = sf::Color::Black;

		oX[0].position = sf::Vector2f(10000000.f, 0.f);
		oX[1].position = sf::Vector2f(-10000000.f, 0.f);

		oY[0].position = sf::Vector2f(0.f, 10000000.f);
		oY[1].position = sf::Vector2f(0.f, -10000000.f);
		
		//True graph
		for (unsigned int i = 0; i < nPoints; ++i) {
			graph[i].position = sf::Vector2f(leftBound + i * dx, -1.0 * function.calc(leftBound + i * dx));
			graph[i].color = sf::Color::Black;
		}
		
		//Rectangle graph
		const double step = (rightBound - leftBound) / nSegments;
		double currentX = leftBound;
		for (unsigned int i = 0, j = 0; i < nSegments; ++i) {
			double yVal = function.calc(currentX + step/2);
			graphRectangle[j].color = sf::Color::Red; graphRectangle[j++].position = sf::Vector2f(currentX, 0.f);
			graphRectangle[j].color = sf::Color::Red; graphRectangle[j++].position = sf::Vector2f(currentX, -yVal);
			currentX += step;
			graphRectangle[j].color = sf::Color::Red; graphRectangle[j++].position = sf::Vector2f(currentX, -yVal);
			graphRectangle[j].color = sf::Color::Red; graphRectangle[j++].position = sf::Vector2f(currentX, 0.f);
		}

		//Trapezoid graph
		currentX = leftBound;
		for (unsigned int i = 0, j = 0; i < nSegments; ++i) {
			double yVal = function.calc(currentX);
			graphTrapezoid[j].color = sf::Color::Green; graphTrapezoid[j++].position = sf::Vector2f(currentX, 0.f);
			graphTrapezoid[j].color = sf::Color::Green; graphTrapezoid[j++].position = sf::Vector2f(currentX, -yVal);
			currentX += step;
			yVal = function.calc(currentX);
			graphTrapezoid[j].color = sf::Color::Green; graphTrapezoid[j++].position = sf::Vector2f(currentX, -yVal);
			graphTrapezoid[j].color = sf::Color::Green; graphTrapezoid[j++].position = sf::Vector2f(currentX, 0.f);
		}
		
		//Simpson graph
		const double h = step / 2;
		currentX = leftBound;
		for (unsigned int i = 0, j = 0; i < nSegments; ++i) {
			currentX = leftBound + step * i;
			const double x2i = currentX + step;
			const double ai = (function.calc(currentX + step) - function.calc(currentX)) / (step * (step - h)) - (function.calc(currentX + h) - function.calc(currentX)) / (h * (step - h)),
				bi = (function.calc(currentX + h) - function.calc(currentX)) / h - ai * (currentX * 2 + h),
				ci = function.calc(currentX) - bi * currentX - ai * currentX * currentX;

			graphSimpson[j].color = sf::Color::Blue; graphSimpson[j++].position = sf::Vector2f(currentX, 0);
			while (currentX < x2i) {
				graphSimpson[j].color = sf::Color::Blue; graphSimpson[j++].position = sf::Vector2f(currentX, -(ai * currentX * currentX + bi * currentX + ci));
				currentX += dx;
			}
			if (i + 1 >= nSegments) {
				while (j < nPoints + nSegments * 2 + 1) graphSimpson[j].position = sf::Vector2f(currentX, 0), graphSimpson[j++].color = sf::Color::Blue;
			}
		}
		
		bool lmbPressed = 0;
		sf::Vector2f oldPos;
		double accZoom = 0.0625, zoom = 0.0625;
		view.zoom(zoom);
		window.setView(view);
		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();

				else if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code) {
					case sf::Keyboard::I:
						zoom = .5f;
						accZoom *= zoom;
						view.zoom(zoom);
						window.setView(view);
						break;

					case sf::Keyboard::O:
						zoom = 2.f;
						accZoom *= zoom;
						view.zoom(zoom);
						window.setView(view);
						break;

					case sf::Keyboard::Num1:
						toggleRectangle = !toggleRectangle;
						break;

					case sf::Keyboard::Num2:
						toggleTrapezoid = !toggleTrapezoid;
						break;

					case sf::Keyboard::Num3:
						toggleSimpson = !toggleSimpson;
						break;

					case sf::Keyboard::Num0:
						toggleGraph = !toggleGraph;
						break;
					}
				}

				else if (event.type == sf::Event::MouseButtonPressed) {
					if (event.mouseButton.button == sf::Mouse::Left) {
						lmbPressed = 1;
						oldPos = sf::Vector2f(sf::Mouse::getPosition());
					}
				}

				else if (event.type == sf::Event::MouseButtonReleased) {
					if (event.mouseButton.button == sf::Mouse::Left) {
						lmbPressed = 0;
					}
				}

				else if (event.type == sf::Event::MouseMoved) {
					if (lmbPressed) {
						const sf::Vector2f newPos = sf::Vector2f(sf::Mouse::getPosition());
						sf::Vector2f dPos = oldPos - newPos;
						dPos.x *= accZoom;
						dPos.y *= accZoom;
						view.move(dPos);
						window.setView(view);
						oldPos = newPos;
					}
				}
			}

			window.clear(sf::Color::White);
			window.draw(oX);
			window.draw(oY);
			if(toggleGraph)		window.draw(graph);
			if(toggleRectangle) window.draw(graphRectangle);
			if(toggleTrapezoid) window.draw(graphTrapezoid);
			if(toggleSimpson)	window.draw(graphSimpson);
			window.display();
		}
		
		std::cin.ignore();
	}

	return 0;
}