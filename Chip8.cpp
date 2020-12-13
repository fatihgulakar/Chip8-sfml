#include "Chip8.h"

Chip8::Chip8(const std::string& filename, const int width, const int height) : window(sf::VideoMode(width, height), "CHIP-8 works!"),
																			   Engine(std::chrono::system_clock::now().time_since_epoch().count()), 
																			   dist(0, 255U)
{
	texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	this->scale_x = static_cast<float>(window.getSize().x) / SCREEN_WIDTH;
	this->scale_y = static_cast<float>(window.getSize().y) / SCREEN_HEIGHT;

	sprite.setTexture(texture);
	sprite.setScale(scale_x, scale_y);

	//dist = std::uniform_int_distribution<uint16_t>(0, 255U);

	//std::copy(fontset.begin(), fontset.end(), memory.begin() + FONTSET_START_ADDRESS);
	for (auto i = 0; i < FONT_SIZE; i++)
		this->memory[FONTSET_START_ADDRESS + i] = fontset[i];

	this->clk.restart();

	this->loadGame(filename);
	//srand(time(NULL));
}

void Chip8::run()
{
	while (this->window.isOpen()) {
		std::cout << "Executing " << std::hex << opcode << " at " << std::hex << pc << std::endl;
		if (this->clk.getElapsedTime().asSeconds() >= 1.0f / 2500.0f) {
			handleEvents();
			emulateCycle();
			draw();
			this->clk.restart();
		}
	}
}

void Chip8::loadGame(const std::string& filename) {


	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open()) {
		std::streampos size = file.tellg();
		//std::cout << "READ SUCCESSFUL. SIZE: " << size << std::endl;
		char* buffer = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		for (auto i = 0; i < size; i++) {
			this->memory[(START_ADRESS + i)] = buffer[i];
		}

		delete[] buffer;
	}
}

void Chip8::handleEvents()
{
	sf::Event event;
	
	
	while (window.pollEvent(event)) {

		if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
			window.close();

		key_list[0] = (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) ? 1 : 0;
		key_list[1] = (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) ? 1 : 0;
		key_list[2] = (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) ? 1 : 0;
		key_list[3] = (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) ? 1 : 0;
		key_list[4] = (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) ? 1 : 0;
		key_list[5] = (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) ? 1 : 0;
		key_list[6] = (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) ? 1 : 0;
		key_list[7] = (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) ? 1 : 0;
		key_list[8] = (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) ? 1 : 0;
		key_list[9] = (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) ? 1 : 0;
		key_list[10] = (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) ? 1 : 0;
		key_list[11] = (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) ? 1 : 0;
		key_list[12] = (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) ? 1 : 0;
		key_list[13] = (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) ? 1 : 0;
		key_list[14] = (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) ? 1 : 0;
		key_list[15] = (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) ? 1 : 0;
	}
	
}

void Chip8::draw()
{
	this->window.clear();
	sf::RectangleShape rect(sf::Vector2f(this->scale_x, this->scale_y));
	for (auto y = 0; y < SCREEN_HEIGHT; y++) {
		for (auto x = 0; x < SCREEN_WIDTH; x++) {
			uint32_t graphics = gfx[x + (64 * y)];
			if (graphics > 0) {
				rect.setPosition(static_cast<float>(x * this->scale_x), static_cast<float>(y * this->scale_y));
				this->window.draw(rect);
			}
		}
	}

	window.display();

	
}

void Chip8::emulateCycle()
{
	// Fetch
	this->opcode = (memory[pc] << 8) | (memory[pc + 1]);
	pc += 2;
	
	uint8_t sum;
	// Decode
	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // 0x00E0 : Clears the screen
			gfx.fill(0);
			//pc += 2;
			break;
		case 0x000E: // 0x00EE : Returns from subroutine
			pc = stack[--sp];
			//pc += 2;
			break;
		default:
			std::cout << "Unknown opcode: " << opcode << std::endl;
			break;
		}
		break;
	case 0x1000: // 1NNN: Jups to address NNN
		pc = opcode & 0x0FFF;
		break;
	case 0x2000: // 2NNN: Calls subroutine NNN
		stack[sp++] = pc;
		pc = opcode & 0x0FFF;
		break;
	case 0x3000: // 3XKK : Skips next instruction if Vx = kk
		pc += (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) ? 2 : 0;
		break;
	case 0x4000: // 4XKK : Skips next instruction if Vx != kk
		pc += (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) ? 2 : 0;
		break;
	case 0x5000: // 5XY0 : Skips next instruction if Vx != Vy
		pc += (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) ? 2 : 0;
		break;
	case 0x6000: // 6XKK : Set Vx == kk
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		//pc += 2;
		break;
	case 0x7000: // 7XKK :  Set Vx = Vx + kk
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		//pc += 2;
		break;
	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // 8XY0 : Set Vx = Vx OR Vy
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			//pc += 2;
			break;
		case 0x0001: // 8XY1 : Set Vx = Vx OR Vy
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			//pc += 2;
			break;
		case 0x0002: // 8XY2 : Set Vx = Vx AND Vy
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0003: // 8XY3 : Set Vx = Vx XOR Vy
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0004: // 8XY4 : Set Vx = Vx + Vy // Kontrol et
			sum = V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4];
			V[0xF] = (sum > 255U) ? 1 : 0;
			V[(opcode & 0x0F00) >> 8] = sum & 0xFF;
			//pc += 2;
			break;
		case 0x0005: // 8XY5 : Set Vx = Vx - Vy
			V[0xF] = (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) ? 1 : 0;
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			//pc += 2;
			break;
		case 0x0006: // 8XY6 : Set Vx = Vx SHR 1
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
			V[(opcode & 0x0F00) >> 8] >>= 1;
			//pc += 2;
			break;
		case 0x0007: // 8XY7 : Set Vx = Vy - Vx
			V[0xF] = (V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4]) ? 1 : 0;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			//pc += 2;
			break;
		case 0x000E: // 8XYE : Set Vx = Vx SHL 1
			V[0xF] = V[((opcode & 0x0F00) >> 8) & 0x80] >> 7;
			V[(opcode & 0x0F00) >> 8] <<= 1;
			//pc += 2;
			break;
		default:
			std::cout << "Unknown opcode: " << opcode << std::endl;
			break;
		}
		break;
	case 0x9000: // 9XY0 : Skip nex instruction if Vx != Vy
		pc += (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) ? 2 : 0;
		break;
	case 0xA000: // ANNN: Sets I to the address NNN
		I = opcode & 0x0FFF;
		//pc += 2;
		break;
	case 0xB000: // BNNN : Jump to location NNN + V0
		pc = (opcode & 0x0FFF) + V[0x0];
		break;
	case 0xC000: // CXKK : Set Vx = random byte AND kk
		V[(opcode & 0x0F00) >> 8] = dist(Engine) & (opcode & 0x00FF);
		pc += 2;
	case 0xD000: // DXYN : Display n-byte sprite starting at memory location I at (Vx, Vy), set Vf = collision
	{
		unsigned short n = opcode & 0x000F;
		unsigned short vx = V[(opcode & 0x0F00) >> 8] % SCREEN_WIDTH; // 2's power olduðu için optimize edilebilir.
		unsigned short vy = V[(opcode & 0x00F0) >> 4] % SCREEN_WIDTH;
		V[0xF] = 0;
		for (auto y = 0; y < n; y++) {
			uint8_t byte = memory[I + y];
			for (auto x = 0; x < 8; x++) {

				uint32_t* spixel = &gfx[(vy + y) * SCREEN_WIDTH + (vx + x)];

				if (byte & (0x80 >> x)) {
					if (*spixel == 0xFFFFFFFF)
						V[0xF] = 1;

					*spixel ^= 0xFFFFFFFF;
				}
			}
		}
		this->drawScreen = true;
		//pc += 2;
		break;
	}
	case 0xE000:
		switch (opcode & 0x00FF)
		{
		case 0x009E: // Ex9E : Skip next instruction if key with the value of Vx is pressed.
			pc += (key_list[V[(opcode & 0x0F00) >> 8]]) ? 2 : 0;
			break;
		case 0x00A1: // ExA1 : Skip next instruction if key with the value of Vx is not pressed.
			pc += (!key_list[V[(opcode & 0x0F00) >> 8]]) ? 2 : 0;
			break;
		default:
			std::cout << "Unknown opcode: " << opcode << std::endl;
			break;
		}
		break;
	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007: // FX07 : Set Vx = delay timer value
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			//pc += 2;
			break;
		case 0x000A: // FX0A : Wait for a key press, store the value of the key in Vx
		{
			auto keyPress = false;
			for (auto i = 0; i <= 0xF; i++) {
				if (key_list[i]) {
					keyPress = true;
					V[(opcode & 0x0F00) >> 8] = i;
					//break;
				}
			}
			if (!keyPress)
				pc -= 2;
			//pc += 2;
			break;
		}
		case 0x0015: // FX15 : Set delay timer = Vx
			delay_timer = V[(opcode & 0x0F00) >> 8];
			//pc += 2;
			break;
		case 0x0018: // FX18 : Set sound timer = Vx
			sound_timer = V[(opcode & 0x0F00) >> 8];
			//pc += 2;
			break;
		case 0x001E: // FX1E : Set I = I + Vx
			V[0xF] = (I + V[(opcode & 0x0F00) >> 8] > 0xFFF) ? 1 : 0;
			I += V[(opcode & 0x0F00) >> 8];
			//pc += 2;
			break;
		case 0x0029: // FX29 : Set I = location of sprite for digit Vx
			I = FONTSET_START_ADDRESS + V[(opcode & 0x0F00) >> 8] * 0x5;
			//pc += 2;
			break;
		case 0x0033: // FX33 : Store BCD representation of Vx in memory locations I, I+1, and I+2 // Kontrol
			memory[I + 2] = (V[(opcode & 0x0F00) >> 8]) % 10;			// Ones digit
			memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;		// Tens digit
			memory[I] = (V[(opcode & 0x0F00) >> 8] / 100) % 10;			// Hundreds digit
			//pc += 2;
			break;
		case 0x0055: // FX55 : Store registers V0 through Vx in memory starting at location I
			for (auto i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
				memory[I + i] = V[i];
			}
			//I += ((opcode & 0x0F00) >> 8) + 1; // TODO: Check
			//pc += 2;
			break;
		case 0x0065: // FX65 : Read registers V0 through Vx in memory starting at location I
			for (auto i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
				V[i] = memory[I + i];
			}
			//I += ((opcode & 0x0F00) >> 8) + 1; // TODO: Check
			//pc += 2;
			break;
		default:
			break;
		}
		break;
	default:
		std::cout << "Unknown opcode: " << opcode << std::endl;
		break;
	}

	// TODO: Update Time func
	if (delay_timer > 0) --delay_timer;
	if (sound_timer > 0) --sound_timer;
}