#include "Computer.h"

Computer::Computer(const std::string& dump_path) : m_program_counter(512)
{
	uint8_t fonts[16 * 5] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for (unsigned int i = 0; i < 16 * 5; ++i)
	{
		m_memory[80 + i] = fonts[i];
	}

	std::ifstream file(dump_path, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		const int size = file.tellg();
		file.seekg(0, std::ios::beg);
		file.read((char*)m_memory + 512u, size);
		file.close();
	}
}

const uint8_t* Computer::getScreen() const
{
	return m_screen;
}

void Computer::buttonPressed(Button button)
{
	m_keyboard[(int)button] = 1;
}

void Computer::buttonReleased(Button button)
{
	m_keyboard[(int)button] = 0;
}

bool Computer::needBeep() const
{
	return m_sound_timer;
}

uint16_t Computer::firstDigit(uint16_t command) const
{
	return (command >> 12);
}

void Computer::step()
{
	const uint16_t first_byte = m_memory[m_program_counter];
	const uint16_t second_byte = m_memory[m_program_counter + 1];

	uint16_t command = first_byte << 8;
	command += second_byte;

	m_program_counter += 2;

	switch (firstDigit(command))
	{
	case 0:
		if (command == 0x00E0)
		{
			// 00E0 - CLS
			CLS();
		}

		else if (command == 0x00EE)
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
		const uint16_t z = command & 0x000F;

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
			// 8xy4 - ADD Vx, Vy		Set Vx = Vx + Vy, set VF = carry
			ADD_2(command);
			break;

		case 5:
			// 8xy5 - SUB Vx, Vy		Set Vx = Vx - Vy, set VF = borrow
			SUB(command);
			break;

		case 6:
			// 8xy6 - SHR Set Vx = Vx SHR 1.
			SHR(command);
			break;

		case 7:
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
		const uint16_t z = command & 0x000F;

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
		const uint16_t fourth_digit = command & 0x000F;
		const uint16_t third_digit = ((command & 0x00F0) >> 4);

		if (third_digit == 0 && fourth_digit == 7)
		{
			// Fx07 - LD Vx, DT
			LD_4(command);
		}

		else if (third_digit == 0 && fourth_digit == 10)
		{
			// Fx0A - LD Vx, K
			LD_5(command);
		}

		else if (third_digit == 1 && fourth_digit == 5)
		{
			//Fx15 - LD DT, Vx 
			LD_6(command);
		}

		else if (third_digit == 1 && fourth_digit == 8)
		{
			//Fx18 - LD ST, Vx
			LD_7(command);
		}

		else if (third_digit == 1 && fourth_digit == 14)
		{
			//Fx1E - ADD I, Vx
			ADD_3(command);
		}

		else if (third_digit == 2 && fourth_digit == 9)
		{
			//Fx29 - LD F, Vx
			LD_8(command);
		}

		else if (third_digit == 3 && fourth_digit == 3)
		{
			//Fx33 - LD B, Vx
			LD_9(command);
		}

		else if (third_digit == 5 && fourth_digit == 5)
		{
			//Fx55 - LD[I], Vx
			LD_10(command);
		}

		else if (third_digit == 6 && fourth_digit == 5)
		{
			//Fx65 - LD Vx, [I]
			LD_11(command);
		}
	}
	break;

	default:
		break;
	}

	if (m_sound_timer)
	{
		--m_sound_timer;
	}
	if (m_delay_timer)
	{
		--m_delay_timer;
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

void Computer::JP_1(uint16_t command)
{
	// 0x1nnn - JP Jump to location nnn
	// 1		n		n		n
	// 0001		....	....	....

	m_program_counter = command & 0x0FFF;
}

void Computer::CALL(uint16_t command)
{
	// 0x2nnn - CALL Call subroutine at nnn.
	// 2		n		n		n
	// 0010		....	....	....

	m_stack[m_stack_pointer] = m_program_counter;
	m_stack_pointer++;
	m_program_counter = command & 0x0FFF;
}

void Computer::SE_1(uint16_t command)
{
	// 0x3xkk - SE Skip next instruction if Vx = kk
	// 3		x		k		k
	// 0011		....	....	....

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	const uint16_t kk = command & 0x00FF;

	if (m_registers[x] == kk)
	{
		m_program_counter += 2;
	}
}

void Computer::SE_2(uint16_t command)
{
	// 0x5xy0 - SE Skip next instruction if Vx = Vy.
	// 5		x		y		0
	// 0101		....	....	0000

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	uint16_t y = command & 0x00F0;
	y = (y >> 4);

	//if (m_registers[x] == y)
	if (m_registers[x] == m_registers[y])
	{
		m_program_counter += 2;
	}
}

void Computer::SNE_1(uint16_t command)
{
	// 0x4xkk - SNE Skip next instruction if Vx != kk.
	// 4		x		k		k
	// 0100		....	....	....

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	const uint16_t kk = command & 0x00FF;

	if (m_registers[x] != kk)
	{
		m_program_counter += 2;
	}
}

void Computer::LD_1(uint16_t command)
{
	// 0x6xkk - LD Set Vx = kk.
	// 6		x		k		k
	// 0110		....	....	....

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	const uint16_t kk = command & 0x00FF;

	m_registers[x] = kk;
}

void Computer::LD_2(uint16_t command)
{
	// 0x8xy0 - LD Set Vx = Vy.
	// 8		x		y		0
	// 1000		....	....	0000

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	uint16_t y = command & 0x00F0;
	y = (y >> 4);

	//m_registers[x] = y;
	m_registers[x] = m_registers[y];
}

void Computer::ADD_1(uint16_t command)
{
	// 0x7xkk - ADD Set Vx = Vx + kk.
	// 7		x		k		k
	// 0111		....	....	....

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	const uint16_t kk = command & 0x00FF;

	//m_registers[x] = x + kk;
	m_registers[x] = m_registers[x] + kk;
}

void Computer::OR(uint16_t command)
{
	// 0x8xy1 - OR Set Vx = Vx OR Vy.
	// 8		x		y		1
	// 1000		....	....	0001

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	uint16_t y = command & 0x00F0;
	y = (y >> 4);

	m_registers[x] = m_registers[x] | m_registers[y];
}

void Computer::AND(uint16_t command)
{
	// 0x8xy2 - AND Set Vx = Vx AND Vy.
	// 8		x		y		2
	// 1000		....	....	0010

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	uint16_t y = command & 0x00F0;
	y = (y >> 4);

	m_registers[x] = m_registers[x] & m_registers[y];
}

void Computer::XOR(uint16_t command)
{
	// 0x8xy3 - XOR Set Vx = Vx XOR Vy.
	// 8		x		y		3
	// 1000		....	....	0011

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	uint16_t y = command & 0x00F0;
	y = (y >> 4);

	m_registers[x] = m_registers[x] ^ m_registers[y];
}

void Computer::ADD_2(uint16_t command)
{
	// 8xy4 - ADD Vx, Vy		Set Vx = Vx + Vy, set VF = ��������� ������������.
	// 8		x		y		4
	// 1000		....	....	0100

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	uint16_t y = command & 0x00F0;
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

void Computer::SUB(uint16_t command)
{
	// 8xy5 - SUB Vx, Vy		Set Vx = Vx - Vy, set VF = carry
	// 8		x		y		5
	// 1000		....	....	0101

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	uint16_t y = command & 0x00F0;
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

void Computer::SUBN(uint16_t command)
{
	// 8xy7 - SUBN Vx, Vy		Set Vx = Vy - Vx, set VF = borrow
	// 8		x		y		7
	// 1000		....	....	0111

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	uint16_t y = command & 0x00F0;
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

void Computer::SHR(uint16_t command)
{
	// 0x8xy6 - SHR Set Vx = Vx SHR 1.
	// 8		x		y		6
	// 1000		....	....	0110

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	m_registers[15] = m_registers[x] & (uint16_t)1;

	m_registers[x] = m_registers[x] >> 1;
}

void Computer::SHL(uint16_t command)
{
	// 0x8xyE - SHL Set Vx = Vx SHL 1.
	// 8		x		y		E
	// 1000		....	....	1110

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	m_registers[15] = m_registers[x] & (uint16_t)128;

	m_registers[x] = m_registers[x] << 1;
}

void Computer::SNE_2(uint16_t command)
{
	// 9xy0 - SNE Vx, Vy Skip next instruction if Vx != Vy.
	// 9		x		y		0
	// 1001		....	....	0000

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	uint16_t y = command & 0x00F0;
	y = (y >> 4);

	if (m_registers[x] != m_registers[y])
	{
		m_program_counter += 2;
	}
}

void Computer::LD_3(uint16_t command)
{
	// Annn - LD I, addr Set I = nnn. I ��� m_index_register
	// A		n		n		n
	// 1010		....	....	....

	m_index_register = (command & 0x0FFF);
}

void Computer::JP_2(uint16_t command)
{
	// Bnnn - JP V0, addr Jump to location nnn + V0.
	// B		n		n		n
	// 1011		....	....	....

	m_program_counter = ((command & 0x0FFF) + m_registers[0]);
}

void Computer::RND(uint16_t command)
{
	// Cxkk - RND Vx, byte Set Vx = random byte AND kk.
	// C		x		k		k
	// 1100		....	....	....

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	const uint16_t kk = command & 0x00FF;

	srand(time(0));

	const uint16_t random_byte = rand() % 256;

	m_registers[x] = (random_byte & kk);
}

void Computer::DRW(uint16_t command)
{
	// Dxyn - DRW Vx, Vy, nibble
	// Display n - byte sprite starting at memory location m_index_register at(Vx, Vy), set VF = collision.
	// D		x		y		n
	// 1101		....	....	.....

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);
	x = m_registers[x];
	x = x % 64;

	uint16_t y = command & 0x00F0;
	y = (y >> 4);
	y = m_registers[y];
	y = y % 32;

	m_registers[15] = 0;

	const uint16_t n = command & 0x000F;

	const uint16_t left_corner = (64 * y + x);

	uint16_t current = left_corner;

	for (int h = 0; h < n; h++)
	{
		int i = 7;
		const uint16_t mask = m_memory[m_index_register + h];

		for (int l = 0; l < 8; l++)
		{
			const uint16_t mask_value = ((mask >> i) & (uint16_t)1);
			if (m_screen[current + l] == 1 && mask_value == 1)
			{
				m_registers[15] = 1;
			}

			m_screen[current + l] = (m_screen[current + l] ^ mask_value);
			i--;
		}

		current += 64;
	}
}

void Computer::SKP(uint16_t command)
{
	// Ex9E - SKP Vx
	// Skip next instruction if key with the value of Vx is pressed.
	// E		x		9		E
	// 1110		....	1001	1110

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	if (m_keyboard[m_registers[x]])
	{
		m_program_counter += 2;
	}
}

void Computer::SKNP(uint16_t command)
{
	// ExA1 - SKNP Vx
	// Skip next instruction if key with the value of Vx is not pressed.
	// E		x		A		1
	// 1110		....	1010	0001

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	if (!m_keyboard[m_registers[x]])
	{
		m_program_counter += 2;
	}
}

void Computer::LD_4(uint16_t command)
{
	// Fx07 - LD Vx, DT
	// Set Vx = delay timer value.
	// F		x		0		7
	// 1111		....	0000	0111

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	m_registers[x] = m_delay_timer;
}

void Computer::LD_5(uint16_t command)
{
	// Fx0A - LD Vx, K
	// Wait for a key press, store the value of the key in Vx.
	// F		x		0		A
	// 1111		....	0000	1010

	uint16_t x = (command & 0x0FFF);
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

void Computer::LD_6(uint16_t command)
{
	//Fx15 - LD DT, Vx 
	//Set delay timer = Vx.
	// F		x		1		5
	// 1111		....	0001	0101

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	m_delay_timer = m_registers[x];
}

void Computer::LD_7(uint16_t command)
{
	//Fx18 - LD ST, Vx
	//Set sound timer = Vx.
	// F		x		1		8
	// 1111		....	0001	1000

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	m_sound_timer = m_registers[x];
}

void Computer::ADD_3(uint16_t command)
{
	//Fx1E - ADD I, Vx
	//Set I = I + Vx.
	// F		x		1		E
	// 1111		....	0001	1110

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	m_index_register = (m_index_register + m_registers[x]);
}

void Computer::LD_8(uint16_t command)
{
	//Fx29 - LD F, Vx
	//Set I = location of sprite for digit Vx.
	// F		x		2		9
	// 1111		....	0010	1001

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	m_index_register = 80 + m_registers[x] * 5;
}

void Computer::LD_9(uint16_t command)
{
	//Fx33 - LD B, Vx
	//Store BCD representation of Vx in memory locations I, I + 1, and I + 2.
	// F		x		3		3
	// 1111		....	0011	0011

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	const uint16_t number = m_registers[x];

	m_memory[m_index_register] = (number / 100);
	m_memory[m_index_register + 1] = ((number % 100) / 10);
	m_memory[m_index_register + 2] = (number % 10);
}

void Computer::LD_10(uint16_t command)
{
	//Fx55 - LD[I], Vx
	//Store registers V0 through Vx in memory starting at location I.
	// F		x		5		5
	// 1111		....	0101	0101

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	for (int i = 0; i <= x; i++)
	{
		m_memory[m_index_register + i] = m_registers[i];
	}
}

void Computer::LD_11(uint16_t command)
{
	//Fx65 - LD Vx, [I]
	//Read registers V0 through Vx from memory starting at location I.
	// F		x		6		5
	// 1111		....	0110	0101

	uint16_t x = (command & 0x0FFF);
	x = (x >> 8);

	for (int i = 0; i <= x; i++)
	{
		m_registers[i] = m_memory[m_index_register + i];
	}
}