#pragma once
#include <string>
#include <cstdint>
#include <fstream>


class Computer
{
public:

	Computer(const std::string& dump_path);

	void step();

private:
	uint8_t firstDigit(const uint16_t* command) const;

	void CLS();

	void RET();

	void JP(const uint16_t* command);

	void CALL(const uint16_t* command);

	void SE_1(const uint16_t* command);

	void SE_2(const uint16_t* command);

	void SNE(const uint16_t* command);

	void LD_1(const uint16_t* command);

	void LD_2(const uint16_t* command);

	void ADD_1(const uint16_t* command);

	void OR(const uint16_t* command);

	void AND(const uint16_t* command);

	void XOR(const uint16_t* command);

	void SHR(const uint16_t* command);

	void SHL(const uint16_t* command);

	// 9xy0 - SNE Vx, Vy Skip next instruction if Vx != Vy.
	// Annn - LD I, addr Set I = nnn. I это m_index_register
	// Bnnn - JP V0, addr Jump to location nnn + V0.
	// Cxkk - RND Vx, byte Set Vx = random byte AND kk.

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
