#pragma once
#include <string>
#include <cstdint>

class Computer
{
public:
	// TODO:
	// ����������� ���������� ����� � ������ ����������
	// ������ ��������� �������� �������� ������
	Computer(const std::string& dump_path);

	// TODO:
	// ��������� ������� (2 �����) 
	// ��������� ������� �� 2
	// ��������� �������
	void step();

private:
	// TODO:
	// 0x00E0 - �������� �����
	// 0x00EE - ������� �� �������

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
