#include "Computer.h"

Computer::Computer(const std::string& dump_path) : m_program_counter(512)
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

uint8_t Computer::firstDigit(const uint16_t* command) const
{
	return ((*command) >> 12);
}


void Computer::step()
{
	const uint16_t* command = reinterpret_cast<uint16_t*>(&m_memory[m_program_counter]);

	m_program_counter += 2;

	switch (firstDigit(command))
	{
	case 0:
		if ((*command) == 0x00E0)
		{
			blankScreen();
		}

		else if ((*command) == 0x00EE)
		{
			returnFromFunction();
		}
		break;

	case 1:
		goToSomeAddress(command);
		break;

	case 2:
		goToFunction(command);
		break;

	case 3:
		skipNextCommand(command);
		break;

	default:
		break;
	}
}

void Computer::blankScreen()
{
	for (int i = 0; i < 1984; i++)
	{
		m_screen[i] = 0;
	}
}

void Computer::returnFromFunction()
{
	m_program_counter = m_stack[m_stack_pointer - 1];
	--m_stack_pointer;
}

void Computer::goToSomeAddress(const uint16_t* command)
{
	m_program_counter = ((*command) ^ (((*command) >> 12) << 12));
}

void Computer::goToFunction(const uint16_t* command)
{
	m_program_counter += 2;
	m_stack[m_stack_pointer] = m_program_counter;
	m_stack_pointer++;
	m_program_counter = ((*command) ^ (((*command) >> 12) << 12));
}

void Computer::skipNextCommand(const uint16_t* command)
{

	uint8_t x = (((*command) ^ (((*command) >> 12) << 12)) >> 8);

	uint8_t kk = (((*command) ^ (((*command) >> 12) << 12)) ^ ((((*command) ^ (((*command) >> 12) << 12)) >> 8) << 8));

	if (m_registers[x] == kk)
	{
		m_program_counter += 2;
	}
}
