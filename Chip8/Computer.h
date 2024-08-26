#pragma once
#include <string>
#include <cstdint>
#include <fstream>

// TODO: дописать
enum class Button
{
	ONE,
	TWO,

};

class Computer
{
public:

	Computer(const std::string& dump_path);

	void step();

	// TODO: возвращать указатель на массив m_screen
	const uint8_t* getScreen() const;

	// TODO: сообщение из внешнего мира о том, что была нажата клавиша
	void buttonPressed(Button button);

	// TODO: сообщение из внешнего мира о том, что клавишу отпустили
	void buttonUnPressed(Button button);

	// TODO: пищим, когда таймер не равен нулю
	bool needBeep() const;

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

	void DRW(const uint16_t* command);

	void SKP(const uint16_t* command);

	void SKNP(const uint16_t* command);

	void LD_4(const uint16_t* command);

	void LD_5(const uint16_t* command);

	// TODO

	//Fx15 - LD DT, Vx 
	//Set delay timer = Vx.

	//Fx18 - LD ST, Vx
	//Set sound timer = Vx.

	//Fx1E - ADD I, Vx
	//Set I = I + Vx.

	//Fx29 - LD F, Vx
	//Set I = location of sprite for digit Vx.

	//Fx33 - LD B, Vx
	//Store BCD representation of Vx in memory locations I, I + 1, and I + 2.

	//Fx55 - LD[I], Vx
	//Store registers V0 through Vx in memory starting at location I.

	//Fx65 - LD Vx, [I]
	//Read registers V0 through Vx from memory starting at location I.

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
