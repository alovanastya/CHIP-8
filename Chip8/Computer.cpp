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
			CLS();
		}

		else if ((*command) == 0x00EE)
		{
			RET();
		}
		break;

	case 1:
		JP(command);
		break;

	case 2:
		CALL(command);
		break;

	case 3:
		SE(command);
		break;

	default:
		break;
	}
}

void Computer::CLS()
{
	for (int i = 0; i < 1984; i++)
	{
		m_screen[i] = 0;
	}
}

void Computer::RET()
{
	m_program_counter = m_stack[m_stack_pointer - 1];
	--m_stack_pointer;
}

void Computer::JP(const uint16_t* command)
{
	// 0x1nnn
	// 1		n		n		n
	// 0001		....	....	....

	m_program_counter = (*command) & 0x0FFF;
}

void Computer::CALL(const uint16_t* command)
{
	// 0x2nnn
	// 2		n		n		n
	// 0010		....	....	....

	m_stack[m_stack_pointer] = m_program_counter;
	m_stack_pointer++;
	m_program_counter = (*command) & 0x00FF;
}

void Computer::SE(const uint16_t* command)
{
	// 0x3xkk
	// 3		x		k		k
	// 0011		....	....	....

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	const uint8_t kk = (*command) & 0x00FF;

	if (m_registers[x] == kk)
	{
		m_program_counter += 2;
	}
}


