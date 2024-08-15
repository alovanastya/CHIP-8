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

	case 4:
		SNE(command);
		break;

	case 5:
		SE(command);
		break;

	case 6:
		LD(command);
		break;

	case 7:
		ADD(command);
		break;

	case 8:
	{
		const uint8_t z = (*command) & 0x000F;

		switch (z)
		{
		case 0:
			LD(command);
			break;

		case 1:
			OR(command);
			break;

		case 2:
			AND(command);
			break;

		case 3:
			XOR(command);
			break;

		case 4:
			// 8xy4 - ADD Vx, Vy		Set Vx = Vx + Vy, set VF = carry.
			break;

		case 5:
			// 8xy5 - SUB Vx, Vy		Set Vx = Vx - Vy, set VF = NOT borrow.
			break;

		case 6:
			SHR(command);
			break;

		case 7:
			// 8xy7 - SUBN Vx, Vy		Set Vx = Vy - Vx, set VF = NOT borrow.
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

	const uint8_t z = (*command) & 0x000F;

	if (z != 0)
	{
		if (m_registers[x] == kk)
		{
			m_program_counter += 2;
		}
	}

	// 5xy0 - SE Vx, Vy			Skip next instruction if Vx = Vy.
	// 
	// 0x5xy0
	// 5		x		y		0
	// 0101		....	....	0000

	else
	{
		const uint8_t y = (*command) & 0x00F0;

		if (m_registers[x] == y)
		{
			m_program_counter += 2;
		}
	}
}

//	Skip next instruction if Vx != kk.
void Computer::SNE(const uint16_t* command)
{
	// 0x4xkk
	// 4		x		k		k
	// 0100		....	....	....

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	const uint8_t kk = (*command) & 0x00FF;

	const uint8_t z = (*command) & 0x000F;

	if (m_registers[x] != kk)
	{
		m_program_counter += 2;
	}
}

//	6xkk - LD Vx, byte		Set Vx = kk.
// 8xy0 - LD Vx, Vy			Set Vx = Vy
void Computer::LD(const uint16_t* command)
{
	// 0x6xkk
	// 6		x		k		k
	// 0110		....	....	....

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	const uint8_t z = (*command) & 0x000F;

	if (z != 0)
	{
		const uint8_t kk = (*command) & 0x00FF;

		m_registers[x] = kk;
	}

	// 0x8xy0
	// 8		x		y		0
	// 1000		....	....	0000

	else
	{
		const uint8_t y = (*command) & 0x00F0;

		m_registers[x] = y;
	}
}

//	7xkk - ADD Vx, byte		Set Vx = Vx + kk.
void Computer::ADD(const uint16_t* command)
{
	// 0x7xkk
	// 7		x		k		k
	// 0111		....	....	....

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	const uint8_t kk = (*command) & 0x00FF;

	m_registers[x] = x + kk;
}

// 8xy1 - OR Vx, Vy			Set Vx = Vx OR Vy.
void Computer::OR(const uint16_t* command)
{
	// 0x8xy1
	// 8		x		y		1
	// 1000		....	....	0001

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	const uint8_t y = (*command) & 0x00F0;

	m_registers[x] = (x | y);
}

// 8xy2 - AND Vx, Vy		Set Vx = Vx AND Vy.
void Computer::AND(const uint16_t* command)
{
	// 0x8xy2
	// 8		x		y		2
	// 1000		....	....	0010

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	const uint8_t y = (*command) & 0x00F0;

	m_registers[x] = (x & y);
}

// 8xy3 - XOR Vx, Vy		Set Vx = Vx XOR Vy.
void Computer::XOR(const uint16_t* command)
{
	// 0x8xy3
	// 8		x		y		3
	// 1000		....	....	0011

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	const uint8_t y = (*command) & 0x00F0;

	m_registers[x] = (x ^ y);
}

// 8xy6 - SHR Vx{ , Vy }	Set Vx = Vx SHR 1.
void Computer::SHR(const uint16_t* command)
{
	// 0x8xy6
	// 8		x		y		6
	// 1000		....	....	0110

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	m_registers[x] = (x >> 1);
}

// 8xyE - SHL Vx{ , Vy }	Set Vx = Vx SHL 1.
void Computer::SHL(const uint16_t* command)
{
	// 0x8xyE
	// 8		x		y		E
	// 1000		....	....	1110

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	m_registers[x] = (x << 1);
}