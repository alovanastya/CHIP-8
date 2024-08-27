#include "Computer.h"
#include <iostream>

int main()
{
	// TODO: ввод пути с клавиатуры в консоли

	std::string dump_path;
	std::cout << "Please enter the path: " << std::endl;
	std::cin >> dump_path;

	Computer computer(dump_path);

	while (true)
	{
	}

	return 0;
}