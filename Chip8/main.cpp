#include <iostream>
#include "Computer.h"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

static const int WINDOW_WIDTH = 640;				// window width (pixels)
static const int WINDOW_HEIGHT = 320;				// window height (pixels)
static const int FREQUENCY = 500;

void processKeys(Computer& computer, sf::RenderWindow& window)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
		}

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.scancode)
			{
			case sf::Keyboard::Scan::Num0:
				computer.buttonPressed(Button::ZERO);
				break;
			case sf::Keyboard::Scan::Num1:
				computer.buttonPressed(Button::ONE);
				break;
			case sf::Keyboard::Scan::Num2:
				computer.buttonPressed(Button::TWO);
				break;
			case sf::Keyboard::Scan::Num3:
				computer.buttonPressed(Button::THREE);
				break;
			case sf::Keyboard::Scan::Num4:
				computer.buttonPressed(Button::FOUR);
				break;
			case sf::Keyboard::Scan::Num5:
				computer.buttonPressed(Button::FIVE);
				break;
			case sf::Keyboard::Scan::Num6:
				computer.buttonPressed(Button::SIX);
				break;
			case sf::Keyboard::Scan::Num7:
				computer.buttonPressed(Button::SEVEN);
				break;
			case sf::Keyboard::Scan::Num8:
				computer.buttonPressed(Button::EIGHT);
				break;
			case sf::Keyboard::Scan::Num9:
				computer.buttonPressed(Button::NINE);
				break;
			case sf::Keyboard::Scan::A:
				computer.buttonPressed(Button::A);
				break;
			case sf::Keyboard::Scan::B:
				computer.buttonPressed(Button::B);
				break;
			case sf::Keyboard::Scan::C:
				computer.buttonPressed(Button::C);
				break;
			case sf::Keyboard::Scan::D:
				computer.buttonPressed(Button::D);
				break;
			case sf::Keyboard::Scan::E:
				computer.buttonPressed(Button::E);
				break;
			case sf::Keyboard::Scan::F:
				computer.buttonPressed(Button::F);
				break;
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.scancode)
			{
			case sf::Keyboard::Scan::Num0:
				computer.buttonReleased(Button::ZERO);
				break;
			case sf::Keyboard::Scan::Num1:
				computer.buttonReleased(Button::ONE);
				break;
			case sf::Keyboard::Scan::Num2:
				computer.buttonReleased(Button::TWO);
				break;
			case sf::Keyboard::Scan::Num3:
				computer.buttonReleased(Button::THREE);
				break;
			case sf::Keyboard::Scan::Num4:
				computer.buttonReleased(Button::FOUR);
				break;
			case sf::Keyboard::Scan::Num5:
				computer.buttonReleased(Button::FIVE);
				break;
			case sf::Keyboard::Scan::Num6:
				computer.buttonReleased(Button::SIX);
				break;
			case sf::Keyboard::Scan::Num7:
				computer.buttonReleased(Button::SEVEN);
				break;
			case sf::Keyboard::Scan::Num8:
				computer.buttonReleased(Button::EIGHT);
				break;
			case sf::Keyboard::Scan::Num9:
				computer.buttonReleased(Button::NINE);
				break;
			case sf::Keyboard::Scan::A:
				computer.buttonReleased(Button::A);
				break;
			case sf::Keyboard::Scan::B:
				computer.buttonReleased(Button::B);
				break;
			case sf::Keyboard::Scan::C:
				computer.buttonReleased(Button::C);
				break;
			case sf::Keyboard::Scan::D:
				computer.buttonReleased(Button::D);
				break;
			case sf::Keyboard::Scan::E:
				computer.buttonReleased(Button::E);
				break;
			case sf::Keyboard::Scan::F:
				computer.buttonReleased(Button::F);
				break;
			}
		}
	}
}

void drawScreen(Computer& computer, sf::RenderWindow& window)
{
	window.clear(sf::Color::Black);

	const uint8_t* screen = computer.getScreen();
	for (int y = 0; y < 32; ++y)
	{
		for (int x = 0; x < 64; ++x)
		{
			if (screen[y * 64 + x])
			{
				sf::RectangleShape pixel({ 10, 10 });
				pixel.setFillColor(sf::Color::White);
				pixel.setPosition({ x * 10.0f, y * 10.0f });
				window.draw(pixel);
			}
		}
	}

	window.display();
}

int main()
{
	std::string dump_path;
	std::cout << "Please enter the path: " << std::endl;
	std::cin >> dump_path;

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "CHIP-8");
	window.setFramerateLimit(FREQUENCY);

	Computer computer(dump_path);

	while (window.isOpen())
	{
		processKeys(computer, window);

		computer.step();

		if (computer.needBeep())
		{
			std::cout << '\a';
		}

		drawScreen(computer, window);
	}

	return 0;
}