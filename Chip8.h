#pragma once
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <array>
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <chrono>
#include <random>


class Chip8
{
public:
	static constexpr uint8_t FONT_SIZE = 80;
	static constexpr uint8_t REG_SIZE = 16;
	static constexpr uint8_t KEY_SIZE = 16;
	static constexpr uint8_t SCREEN_WIDTH = 64;
	static constexpr uint8_t SCREEN_HEIGHT = 32;
	static constexpr uint8_t STACK_SIZE = 16;
	static constexpr uint16_t MEM_SIZE = 4096;
	static constexpr uint16_t SCREEN_SIZE = (SCREEN_WIDTH * SCREEN_HEIGHT);
	static constexpr uint16_t START_ADRESS = 0x200;
	static constexpr uint16_t FONTSET_START_ADDRESS = 0x50;
	Chip8(const std::string& filename, const int width = 1600, const int height = 900);
	void run();
private:
	std::default_random_engine Engine;
	std::uniform_int_distribution<uint16_t> dist;
	bool drawScreen = true;
	uint8_t sp{ 0 };
	uint16_t I{ 0 };
	uint16_t opcode{ 0 };
	uint16_t pc{ START_ADRESS };
	uint8_t delay_timer{ 0 };
	uint8_t sound_timer{ 0 };

	std::array<uint8_t, MEM_SIZE> memory{ 0 };
	std::array<uint8_t, REG_SIZE> V{ 0 };
	std::array<uint8_t, KEY_SIZE> key_list{ 0 };
	std::array<uint32_t, SCREEN_SIZE> gfx{ 0 };
	std::array<uint16_t, STACK_SIZE> stack{ 0 };

	

	static constexpr std::array<uint8_t, FONT_SIZE> fontset =
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



	void loadGame(const std::string& filename);
	void handleEvents();
	void emulateCycle();
	void draw();
	sf::RenderWindow window;
	sf::Clock clk;
	sf::Texture texture;
	sf::Sprite sprite;
	bool drawFlag = false;;
	bool running = true;
	float scale_x;
	float scale_y;
	
};

