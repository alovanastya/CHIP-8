#pragma once
#include <string>
#include <cstdint>
#include <fstream>

class Computer
{
public:
	// TODO:
	// ����������� ���������� ����� � ������ ����������
	// ������ ��������� �������� �������� ������
	Computer(const std::string& dump_path)
	{
		std::ifstream file(dump_path, std::ios::binary);

		if (file.is_open())
		{
			int i = 0;

			while (!file.eof())
			{
				file >> m_memory[i]; i++;
			}
		}
		file.close();
		/*
		�� 0 �� 511 ������������ - ����������� ������� ������,
		�� � 80 �� 160 ����� 16 �������� � ���� ������� �����
		�� 512 �� 4095 ������������ - ����� ��� ���� ���������
		*/
		m_program_counter = 512;
	}

	// TODO:
	// ��������� ������� (2 �����) 
	// ��������� ������� �� 2
	// ��������� �������
	void step()
	{
		//int command = m_memory[m_program_counter];

		m_program_counter += 2;

		//switch (command)
		//{
		//case 0x00E0:
		//	blankScreen();
		//	break;

		//case 0x00EE:
		//	returnFromFunction();
		//	break;

		//default:
		//	break;
		//}
	}

private:
	// TODO:
	// 0x00E0 - �������� �����
	// 0x00EE - ������� �� �������

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
	}

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
