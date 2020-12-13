// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Chip8.h"
#include <iostream>



int main()
{
    Chip8 chip("./roms/space_invaders.ch8");

    
    chip.run();
    

    return 0;
}