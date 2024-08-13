#pragma once
#include <string>
#include <cstdint>
#include <fstream>

// TODO: ��������� � .cpp

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

	// TODO: ����������� �������
	// 0x1nnn - ����������� �� ������ nnn
	// 0x2nnn - ������� � ������������ �� ������ nnn
	// 0x3xkk - ���������� ��������� �������, ���� ������� � ������� x ����� kk

	// �������: ��� ������, ��� � ������� ������� ����������������� ������ ����� 1, 2 ��� 3?
	// ��� �������� ������� ��������� ���������� ���������� �����, 
	// ������� ����� ������������ � ������� x �������� ����� 2^x

	// ��� ����������������� ������� ��������� ���������� ���������� �����,
	// ������� ����� ������������ � ������� x �������� ����� 16^x

	// ����� ���� ������������������ ����� �� 4 ��������
	// xyzw
	// ������� ���������� ����� ��� ����� ������������? 65536 ������
	// � �������� ������� ��������� ��� �� ������������� 16 ��������
	// ����������, ��� ���� ������ ������������������ ����� � 4 ���� "�������" ������ ������� ��������� ����� 
	// ����� ���� ������ ������������������ ����� ������������� 4 �������� ��������� � �����
	// ����������� ������ ����� � ����������������� � �������� ������ ��������� �������
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
