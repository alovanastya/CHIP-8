#pragma once
#include <string>
#include <cstdint>
#include <fstream>

// TODO: ��������� � .cpp

class Computer
{
public:

	Computer(const std::string& dump_path) : m_program_counter(512)
	{
		std::ifstream file(dump_path, std::ios::binary);

		if (file.is_open())
		{
			int i = 512;

			while (!file.eof())
			{
				file >> m_memory[i];
				i++;

				if (i >= 4096)
				{
					break;
				}
			}

			file.close();
		}
	}

	void step()
	{
		const uint16_t* command = reinterpret_cast<uint16_t*>(&m_memory[m_program_counter]);

		m_program_counter += 2;

		switch (*command)
		{
		case 0x00E0:
			blankScreen();
			break;

		case 0x00EE:
			returnFromFunction();
			break;

		default:
			break;
		}
	}

private:
	void blankScreen()
	{
		for (int i = 0; i < 1984; i++)
		{
			m_screen[i] = 0;
		}
	}

	void returnFromFunction()
	{
		m_program_counter = m_stack[m_stack_pointer - 1];
		--m_stack_pointer;
	}

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
