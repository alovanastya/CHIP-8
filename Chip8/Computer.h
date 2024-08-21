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

	void JP_1(const uint16_t* command);

	void CALL(const uint16_t* command);

	void SE_1(const uint16_t* command);

	void SE_2(const uint16_t* command);

	void SNE_1(const uint16_t* command);

	void LD_1(const uint16_t* command);

	void LD_2(const uint16_t* command);

	void ADD_1(const uint16_t* command);

	void OR(const uint16_t* command);

	void AND(const uint16_t* command);

	void XOR(const uint16_t* command);

	void SHR(const uint16_t* command);

	void SHL(const uint16_t* command);

	void SNE_2(const uint16_t* command);

	void LD_3(const uint16_t* command);

	void JP_2(const uint16_t* command);

	void RND(const uint16_t* command);

	void ADD_2(const uint16_t* command);

	void SUB(const uint16_t* command);

	void SUBN(const uint16_t* command);

	// TODO:
	// у нас есть экран 64 пикселя в ширину и 32 в высоту. Пиксель может быть черным либо белым (0/1)
	// Мы работаем с экраном, рисуя на нем спрайты (т.е. накладывая на него битовые маски)
	// При отрисовке спрайта на экране выполнятся операция XOR
	// Спрайты хранятся в памяти в виде прямугольных битовых масок. Ширина битовой маски всегда равна 8 битам,
	// а высота может быть от 1 до 15 (см телеграм)
	// (0,0) -x (63, 0)
	// |
	// y
	// (0,31)   (63,31)
	// Dxyn - DRW Vx, Vy, nibble
	// Display n - byte sprite starting at memory location m_index_register at(Vx, Vy), set VF = collision.
	
	// Ex9E - SKP Vx
	// Skip next instruction if key with the value of Vx is pressed.

	// ExA1 - SKNP Vx
	// Skip next instruction if key with the value of Vx is not pressed.

	// Fx07 - LD Vx, DT
	// Set Vx = delay timer value.

	// Fx0A - LD Vx, K
	// Wait for a key press, store the value of the key in Vx.

private:
	uint16_t m_program_counter;

	uint8_t m_registers[16];
	uint16_t m_index_register;

	uint8_t m_memory[4096];

	uint16_t m_stack[16];
	uint8_t m_stack_pointer;

	uint8_t m_sound_timer;
	uint8_t m_delay_timer;

	uint8_t m_screen[62 * 32];

	bool m_keyboard[16];
};
