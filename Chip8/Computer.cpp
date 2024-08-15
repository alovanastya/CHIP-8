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
			// 00E0 - CLS
			CLS();
		}

		else if ((*command) == 0x00EE)
		{
			// 00EE - RET
			RET();
		}
		break;

	case 1:
		// 1nnn - JP addr
		JP(command);
		break;

	case 2:
		// 2nnn - CALL addr
		CALL(command);
		break;

	case 3:
		// 3xkk - SE Vx, byte
		SE_1(command);
		break;

	case 4:
		// 4xkk - SNE Vx, byte
		SNE(command);
		break;

	case 5:
		// 5xy0 - SE Vx, Vy
		SE_2(command);
		break;

	case 6:
		// 6xkk - LD Vx, byte
		LD_1(command);
		break;

	case 7:
		// 7xkk - ADD Vx, byte
		ADD_1(command);
		break;

	case 8:
	{
		const uint8_t z = (*command) & 0x000F;

		switch (z)
		{
		case 0:
			// 8xy0 - LD Vx, Vy
			LD_2(command);
			break;

		case 1:
			// 8xy1 - OR Vx, Vy
			OR(command);
			break;

		case 2:
			// 8xy2 - AND Vx, Vy
			AND(command);
			break;

		case 3:
			// 8xy3 - XOR Vx, Vy
			XOR(command);
			break;

		case 4:
			// TODO
			// 8xy4 - ADD Vx, Vy		Set Vx = Vx + Vy, set VF = произошло переполнение.
			break;

		case 5:
			// TODO
			// 8xy5 - SUB Vx, Vy		Set Vx = Vx - Vy, set VF = результат не отрицательный
			break;

		case 6:
			SHR(command);
			break;

		case 7:
			// TODO
			// 8xy7 - SUBN Vx, Vy		Set Vx = Vy - Vx, set VF = результат не отрицательный
			break;

		case 14:
			SHL(command);
			break;

		default:
			break;
		}
	}
	break;

	default:
		break;
	}
}

void Computer::CLS()
{
	// 0x00E0 - CLS Clear the display.
	for (int i = 0; i < 1984; i++)
	{
		m_screen[i] = 0;
	}
}

void Computer::RET()
{
	// 0x00EE - RET Return from a subroutine.
	m_program_counter = m_stack[m_stack_pointer - 1];
	--m_stack_pointer;
}

void Computer::JP(const uint16_t* command)
{
	// 0x1nnn - JP Jump to location nnn
	// 1		n		n		n
	// 0001		....	....	....

	m_program_counter = (*command) & 0x0FFF;
}

void Computer::CALL(const uint16_t* command)
{
	// 0x2nnn - CALL Call subroutine at nnn.
	// 2		n		n		n
	// 0010		....	....	....

	m_stack[m_stack_pointer] = m_program_counter;
	m_stack_pointer++;
	m_program_counter = (*command) & 0x00FF;
}

void Computer::SE_1(const uint16_t* command)
{
	// 0x3xkk - SE Skip next instruction if Vx = kk
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

void Computer::SE_2(const uint16_t* command)
{
	// 0x5xy0 - SE Skip next instruction if Vx = Vy.
	// 5		x		y		0
	// 0101		....	....	0000

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	uint8_t y = (*command) & 0x00F0;
	y = (y >> 4);

	if (m_registers[x] == y)
	{
		m_program_counter += 2;
	}
}

void Computer::SNE(const uint16_t* command)
{
	// 0x4xkk - SNE Skip next instruction if Vx != kk.
	// 4		x		k		k
	// 0100		....	....	....

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	const uint8_t kk = (*command) & 0x00FF;

	if (m_registers[x] != kk)
	{
		m_program_counter += 2;
	}
}

void Computer::LD_1(const uint16_t* command)
{
	// 0x6xkk - LD Set Vx = kk.
	// 6		x		k		k
	// 0110		....	....	....

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	const uint8_t kk = (*command) & 0x00FF;

	m_registers[x] = kk;
}

void Computer::LD_2(const uint16_t* command)
{
	// 0x8xy0 - LD Set Vx = Vy.
	// 8		x		y		0
	// 1000		....	....	0000

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	uint8_t y = (*command) & 0x00F0;
	y = (y >> 4);

	m_registers[x] = y;
}

void Computer::ADD_1(const uint16_t* command)
{
	// 0x7xkk - ADD Set Vx = Vx + kk.
	// 7		x		k		k
	// 0111		....	....	....

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	const uint8_t kk = (*command) & 0x00FF;

	m_registers[x] = x + kk;
}

void Computer::OR(const uint16_t* command)
{
	// 0x8xy1 - OR Set Vx = Vx OR Vy.
	// 8		x		y		1
	// 1000		....	....	0001

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	uint8_t y = (*command) & 0x00F0;
	y = (y >> 4);

	m_registers[x] = m_registers[x] | m_registers[y];
}

void Computer::AND(const uint16_t* command)
{
	// 0x8xy2 - AND Set Vx = Vx AND Vy.
	// 8		x		y		2
	// 1000		....	....	0010

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	uint8_t y = (*command) & 0x00F0;
	y = (y >> 4);

	m_registers[x] = m_registers[x] & m_registers[y];
}

void Computer::XOR(const uint16_t* command)
{
	// 0x8xy3 - XOR Set Vx = Vx XOR Vy.
	// 8		x		y		3
	// 1000		....	....	0011

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	uint8_t y = (*command) & 0x00F0;
	y = (y >> 4);

	m_registers[x] = m_registers[x] ^ m_registers[y];
}

void Computer::SHR(const uint16_t* command)
{
	// 0x8xy6 - SHR Set Vx = Vx SHR 1.
	// 8		x		y		6
	// 1000		....	....	0110

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	// TODO: обновить VF - записать туда число, которое "выпадет" после сдвига

	m_registers[x] = m_registers[x] >> 1;
}

void Computer::SHL(const uint16_t* command)
{
	// 0x8xyE - SHL Set Vx = Vx SHL 1.
	// 8		x		y		E
	// 1000		....	....	1110

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	// TODO: обновить VF - записать туда число, которое "выпадет" после сдвига

	m_registers[x] = (x << 1);
}