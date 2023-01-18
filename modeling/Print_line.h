#pragma once

#include <Windows.h>
#include <iostream>
#include <string>

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
COORD cursorPos{ 0, 0 };

void Print_line(std::string str, int num_line)
{
	cursorPos.Y = num_line;
	SetConsoleCursorPosition(hStdOut, cursorPos);

	std::cout << str;
}