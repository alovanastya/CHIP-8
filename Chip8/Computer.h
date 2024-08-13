#pragma once
#include <string>
#include <cstdint>
#include <fstream>

// TODO: перенести в .cpp

class Computer
{
public:

	Computer(const std::string& dump_path);

	uint8_t firstDigit(const uint16_t* command) const;

	void step();

private:
	void blankScreen();

	void returnFromFunction();

	void goToSomeAddress(const uint16_t* command);

	void goToFunction(const uint16_t* command);

	void skipNextCommand(const uint16_t* command);

	// TODO: реализовать команды
	// 0x1nnn - перескочить по адресу nnn
	// 0x2nnn - перейти в подпрограмму по адресу nnn
	// 0x3xkk - пропустить следующую команду, если регистр с номером x равен kk

	// Справка: как понять, что в старшем разряде шестнадцатеричной записи стоит 1, 2 или 3?
	// для двоичной системы счисления количество уникальных чисел, 
	// которые можно закодировать с помощью x разрядов равно 2^x

	// для шестнадцатеричной системы счисления количество уникальных чисел,
	// которые можно закодировать с помощью x разрядов равно 16^x

	// пусть есть шестнадцатеричного число из 4 разрядов
	// xyzw
	// сколько уникальных чисел оно может закодировать? 65536 числел
	// в двоичной системе счисления нам бы потребовалось 16 разрядов
	// получается, что один разряд шестнадцатеричного числа в 4 раза "сильнее" одного разряда двоичного числа 
	// тогда один разряд шестнадцатеричного числа соответствует 4 разрядам двоичного и можно
	// сопоставить запись числа в шестнадцатеричной и двоичной записи следующим образом
	//	x		y		z		w
	//	0101	0101	0101	0101

private:
	uint16_t m_program_counter;

	uint8_t m_registers[15];
	uint8_t m_state_register;
	uint16_t m_index_register;

	uint8_t m_memory[4096];

	uint16_t m_stack[16];
	uint8_t m_stack_pointer;

	uint8_t m_sound_timer;
	uint8_t m_delay_timer;

	uint8_t m_screen[62 * 32];

	uint8_t m_keyboard[16];
};
