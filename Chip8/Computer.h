#pragma once
#include <string>
#include <cstdint>
#include <fstream>

// TODO: перенести в .cpp

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

	void SE(const uint16_t* command);

	// TODO: реализовать команды
	// 4xkk - SNE Vx, byte		Skip next instruction if Vx != kk.
	// 5xy0 - SE Vx, Vy			Skip next instruction if Vx = Vy.
	// 6xkk - LD Vx, byte		Set Vx = kk.
	// 7xkk - ADD Vx, byte		Set Vx = Vx + kk.
	// 8xy0 - LD Vx, Vy			Set Vx = Vy
	// 8xy1 - OR Vx, Vy			Set Vx = Vx OR Vy.
	// 8xy2 - AND Vx, Vy		Set Vx = Vx AND Vy.
	// 8xy3 - XOR Vx, Vy		Set Vx = Vx XOR Vy.
	// 8xy6 - SHR Vx{ , Vy }	Set Vx = Vx SHR 1.
	// 8xyE - SHL Vx{ , Vy }	Set Vx = Vx SHL 1.

	// 8xy4 - ADD Vx, Vy		Set Vx = Vx + Vy, set VF = carry.
	// 8xy5 - SUB Vx, Vy		Set Vx = Vx - Vy, set VF = NOT borrow.
	// 8xy7 - SUBN Vx, Vy		Set Vx = Vy - Vx, set VF = NOT borrow.



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
