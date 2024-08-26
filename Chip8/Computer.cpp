#include "Computer.h"
#include <time.h>

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
		JP_1(command);
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
		SNE_1(command);
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
			ADD_2(command);
			break;

		case 5:
			// TODO
			// 8xy5 - SUB Vx, Vy		Set Vx = Vx - Vy, set VF = результат не отрицательный
			SUB(command);
			break;

		case 6:
			// 8xy6 - SHR Set Vx = Vx SHR 1.
			SHR(command);
			break;

		case 7:
			// TODO
			// 8xy7 - SUBN Vx, Vy
			SUBN(command);
			break;

		case 14:
			// 8xyE - SHL Set Vx = Vx SHL 1.
			SHL(command);
			break;

		default:
			break;
		}
	}
	break;

	case 9:
		// 9xy0 - SNE Vx, Vy
		SNE_2(command);
		break;

	case 10:
		// Annn - LD I, addr
		LD_3(command);
		break;

	case 11:
		// Bnnn - JP V0, addr Jump to location nnn + V0.
		JP_2(command);
		break;

	case 12:
		// Cxkk - RND Vx, byte
		RND(command);
		break;

	case 13:
		// Dxyn - DRW Vx, Vy, nibble
		DRW(command);
		break;

	case 14:
	{
		const uint8_t z = (*command) & 0x000F;

		switch (z)
		{
		case 1:
			// ExA1 - SKNP Vx
			SKNP(command);
			break;

		case 14:
			// Ex9E - SKP Vx
			SKP(command);
			break;

		default:
			break;
		}
	}
		break;

	case 15:
	{
		const uint8_t z = (*command) & 0x000F;

		switch (z)
		{
		case 7:
			// Fx07 - LD Vx, DT
			LD_4(command);
			break;

		case 10:
			// Fx0A - LD Vx, K
			LD_5(command);
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

void Computer::JP_1(const uint16_t* command)
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

	//if (m_registers[x] == y)
	if (m_registers[x] == m_registers[y])
	{
		m_program_counter += 2;
	}
}

void Computer::SNE_1(const uint16_t* command)
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

	//m_registers[x] = y;
	m_registers[x] = m_registers[y];
}

void Computer::ADD_1(const uint16_t* command)
{
	// 0x7xkk - ADD Set Vx = Vx + kk.
	// 7		x		k		k
	// 0111		....	....	....

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	const uint8_t kk = (*command) & 0x00FF;

	//m_registers[x] = x + kk;
	m_registers[x] = m_registers[x] + kk;
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

void Computer::ADD_2(const uint16_t* command)
{
	// 8xy4 - ADD Vx, Vy		Set Vx = Vx + Vy, set VF = произошло переполнение.
	// 8		x		y		4
	// 1000		....	....	0100

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	uint8_t y = (*command) & 0x00F0;
	y = (y >> 4);

	m_registers[x] = m_registers[x] + m_registers[y];

	if (m_registers[x] + m_registers[y] > 255) 
	{
		m_registers[15] = 1;
	}

	else
	{
		m_registers[15] = 0;
	}
}

void Computer::SUB(const uint16_t* command)
{
	// 8xy5 - SUB Vx, Vy		Set Vx = Vx - Vy, set VF = результат не отрицательный
	// 8		x		y		5
	// 1000		....	....	0101

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	uint8_t y = (*command) & 0x00F0;
	y = (y >> 4);

	m_registers[x] = (m_registers[x] - m_registers[y]);

	if ((int)m_registers[x] - (int)m_registers[y] < 0)
	{
		m_registers[15] = 0;
	}

	else
	{
		m_registers[15] = 1;
	}
}

void Computer::SUBN(const uint16_t* command)
{
	// 8xy7 - SUBN Vx, Vy		Set Vx = Vy - Vx, set VF = результат не отрицательный
	// 8		x		y		7
	// 1000		....	....	0111

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	uint8_t y = (*command) & 0x00F0;
	y = (y >> 4);

	m_registers[x] = (m_registers[y] - m_registers[x]);

	if ((int)m_registers[y] - (int)m_registers[x] < 0)
	{
		m_registers[15] = 0;
	}

	else
	{
		m_registers[15] = 1;
	}
}

void Computer::SHR(const uint16_t* command)
{
	// 0x8xy6 - SHR Set Vx = Vx SHR 1.
	// 8		x		y		6
	// 1000		....	....	0110

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	// TODO: обновить VF - записать туда число, которое "выпадет" после сдвига

	m_registers[15] = m_registers[x] & (uint8_t)1;

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

	m_registers[15] = m_registers[x] & (uint8_t)128;

	m_registers[x] = m_registers[x] << 1;
}

void Computer::SNE_2(const uint16_t* command)
{
	// 9xy0 - SNE Vx, Vy Skip next instruction if Vx != Vy.
	// 9		x		y		0
	// 1001		....	....	0000

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	uint8_t y = (*command) & 0x00F0;
	y = (y >> 4);

	if (m_registers[x] != m_registers[y])
	{
		m_program_counter += 2;
	}
}

void Computer::LD_3(const uint16_t* command)
{
	// Annn - LD I, addr Set I = nnn. I это m_index_register
	// A		n		n		n
	// 1010		....	....	....

	m_index_register = ((*command) & 0x0FFF);
}

void Computer::JP_2(const uint16_t* command)
{
	// Bnnn - JP V0, addr Jump to location nnn + V0.
	// B		n		n		n
	// 1011		....	....	....

	m_program_counter = (((*command) & 0x0FFF) + m_registers[0]);
}

void Computer::RND(const uint16_t* command)
{
	// Cxkk - RND Vx, byte Set Vx = random byte AND kk.
	// C		x		k		k
	// 1100		....	....	....

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	const uint8_t kk = (*command) & 0x00FF;
	
	srand(time(0));

	const uint8_t random_byte = rand() % 256;

	m_registers[x] = (random_byte & kk);
}

void Computer::DRW(const uint16_t* command)
{
	// Dxyn - DRW Vx, Vy, nibble
	// Display n - byte sprite starting at memory location m_index_register at(Vx, Vy), set VF = collision.
	// D		x		y		n
	// 1101		....	....	.....

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	uint8_t y = (*command) & 0x00F0;
	y = (y >> 4);

	const uint8_t n = (*command) & 0x000F;

	const uint16_t left_corner = (64 * x + y);

	uint16_t current = left_corner;

	for (int h = 0; h < n; h++)
	{
		int i = 7;

		uint8_t mask = m_registers[m_index_register + h];

		for (int l = 0; l < 8; l++)
		{

			if (m_screen[current + l] == 1 && ((mask >> i) & (uint8_t)1) == 1)
			{
				m_registers[15] = 1;
			}

			m_screen[current + l] = (m_screen[current + l] ^ ((mask >> i) & (uint8_t)1));
			i--;
		}
		current += 64;
	}
}

void Computer::SKP(const uint16_t* command)
{
	// Ex9E - SKP Vx
	// Skip next instruction if key with the value of Vx is pressed.
	// E		x		9		E
	// 1110		....	1001	1110

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	if (m_keyboard[m_registers[x]] == 1)
	{
		m_program_counter += 2;
	}
}

void Computer::SKNP(const uint16_t* command)
{
	// ExA1 - SKNP Vx
	// Skip next instruction if key with the value of Vx is not pressed.
	// E		x		A		1
	// 1110		....	1010	0001

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	if (m_keyboard[m_registers[x]] == 0)
	{
		m_program_counter += 2;
	}
}

void Computer::LD_4(const uint16_t* command)
{
	// Fx07 - LD Vx, DT
	// Set Vx = delay timer value.
	// F		x		0		7
	// 1111		....	0000	0111

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	m_registers[x] = m_delay_timer;
}

void Computer::LD_5(const uint16_t* command)
{
	// Fx0A - LD Vx, K
	// Wait for a key press, store the value of the key in Vx.
	// F		x		0		A
	// 1111		....	0000	1010

	uint8_t x = ((*command) & 0x0FFF);
	x = (x >> 8);

	bool is_pressed = false;

	for (int i = 0; i < 16; i++)
	{
		if (m_keyboard[i] == 1)
		{
			is_pressed = true;
			m_registers[x] = i;
		}
	}

	if (!is_pressed)
	{
		m_program_counter -= 2;
	}
}