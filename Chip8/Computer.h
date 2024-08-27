#pragma once
#include <string>
#include <cstdint>
#include <fstream>

// TODO: ��������
enum class Button
{
	ZERO,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	A,
	B,
	C,
	D,
	E,
	F
};

class Computer
{
public:

	Computer(const std::string& dump_path);

	void step();

	const uint8_t* getScreen() const;

	void buttonPressed(Button button);

	void buttonReleased(Button button);

	bool needBeep() const;

private:
	uint16_t firstDigit(uint16_t command) const;

	void CLS();

	void RET();

	void JP_1(uint16_t command);

	void CALL(uint16_t command);

	void SE_1(uint16_t command);

	void SE_2(uint16_t command);

	void SNE_1(uint16_t command);

	void LD_1(uint16_t command);

	void LD_2(uint16_t command);

	void ADD_1(uint16_t command);

	void OR(uint16_t command);

	void AND(uint16_t command);

	void XOR(uint16_t command);

	void SHR(uint16_t command);

	void SHL(uint16_t command);

	void SNE_2(uint16_t command);

	void LD_3(uint16_t command);

	void JP_2(uint16_t command);

	void RND(uint16_t command);

	void ADD_2(uint16_t command);

	void SUB(uint16_t command);

	void SUBN(uint16_t command);

	void DRW(uint16_t command);

	void SKP(uint16_t command);

	void SKNP(uint16_t command);

	void LD_4(uint16_t command);

	void LD_5(uint16_t command);

	void LD_6(uint16_t command);

	void LD_7(uint16_t command);

	void ADD_3(uint16_t command);

	void LD_8(uint16_t command);

	void LD_9(uint16_t command);

	void LD_10(uint16_t command);

	void LD_11(uint16_t command);

private:
	uint16_t m_program_counter;

	uint8_t m_registers[16] = { 0u };
	uint16_t m_index_register = 0u;

	uint8_t m_memory[4096];

	uint16_t m_stack[16] = { 0u };
	uint8_t m_stack_pointer = 0u;

	uint8_t m_sound_timer = 0u;
	uint8_t m_delay_timer = 0u;

	uint8_t m_screen[64 * 32] = { 0u };

	bool m_keyboard[16] = { false };
};
