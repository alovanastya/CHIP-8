#pragma once
#include <string>
#include <cstdint>

class Computer
{
public:
	// TODO:
	// скопировать содержимое дампа в память компьютера
	// задать начальное значение счетчика команд
	Computer(const std::string& dump_path);

	// TODO:
	// прочитать команду (2 байта) 
	// увеличить счетчик на 2
	// выполнить команду
	void step();

private:
	// TODO:
	// 0x00E0 - очистить экран
	// 0x00EE - возврат из функции

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
