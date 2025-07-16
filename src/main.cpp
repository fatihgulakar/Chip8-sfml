#include "Chip8.h"
#include <iostream>

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <path_to_rom>\n";
    return 1;
  }

  std::string rom_path = argv[1];

  Chip8 chip(rom_path);
  chip.run();

  return 0;
}